/** @file
 **
 ** Converts status messages from/to old robots.
 **
 */

#include "referee.h"
#include "motion/motion.h"
#include "system/timer.h"
#include "robot.h"
#include "comm.h"
#include "config/configRegistry.h"
#include "debug.h"
#include "worldModel/worldModel.h"
#include "motorbus/motorbus.h"

#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include <string>


REGISTER_OPTION("game.team.id",    DEFAULTTEAMID,         "Team ID for game controller");
REGISTER_OPTION("referee.port",    REFEREEPORT,           "UDP port for referee / game controller messages");
REGISTER_OPTION("referee.enabled", DEFAULTREFEREEENABLED, "Whether the referee is enabled");


/*------------------------------------------------------------------------------------------------*/

/**
 ** Constructor
**/

RefereeGameController::RefereeGameController()
	: transport(0)
	, teamID(-1)
	, refereeEnabled(false)
	, ourLastScore(0)
	, oppLastScore(0)
	, GCinitialized(false)

{
	cs.setName("RefereeGameController::cs");
}

void RefereeGameController::init() {
	CriticalSectionLock lock(cs);

	// get team ID
	teamID = robot.getConfig().getIntValue("game.team.id", DEFAULTTEAMID);
	wm.setTeamID(teamID);

	// enable referee unless turned off
	refereeEnabled = (0 != robot.getConfig().getIntValue("referee.enabled", DEFAULTREFEREEENABLED));

	if (!refereeEnabled) {
		WARNING("Referee is disabled");
		return;
	}

	transport = new TransportUDP(robot.getConfig().getIntValue("referee.port", REFEREEPORT), false);
	if (transport == 0) {
		WARNING("Could not open UDP socket for referee, referee is disabled");
		return;
	}

	if (false == transport->open()) {
		WARNING("Could not open UDP connection for referee, referee is disabled");
		return;
	}

	// start thread
	run();
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Destructor
**/

RefereeGameController::~RefereeGameController() {
	kill();

	// close transport
	if (transport) {
		transport->close();
		delete transport;
	}

	transport = 0;
}


/*------------------------------------------------------------------------------------------------*/

/**
 ** Handles incoming data.
 **
 ** The thread code is a simple loop receiving messages and dispatching them to handleOperation().
 ** Note that messages are processed in the order they are received, and one after the other.
**/

#define BUFFSIZE 1500

void RefereeGameController::threadMain() {
	while (isRunning()) {
		if (false == transport->waitForData(1, 500*1000 /* µs */))
			continue;

		struct sockaddr_in remoteAddress;
		uint8_t buffer[BUFFSIZE];

		int received = transport->read(buffer, BUFFSIZE, &remoteAddress);
		if (received <= 0) {
			// TODO: handle error in receiving
			printf("error recv: %d - %s\n", errno, strerror(errno));
			continue;
		}

		if (strncmp((const char*)buffer, STRUCT_HEADER, 4) == 0) {
			if (received != sizeof(RoboCupGameControlData))
				ERROR("Received referee data with size mismatch, got %d but expected %d bytes", received, (int)sizeof(RoboCupGameControlData));
			else
				handleRefereeMessage((RoboCupGameControlData*)(void*)buffer);
		} else {
//			printf("received unknown packet, header is %x%x%x%x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
		}
	}
}


/*------------------------------------------------------------------------------------------------*/

/** Convert a robot's ID to the one used by the game controller
 **
 ** @param robotID    The ID of the robot in our system
 ** @return ID of the robot in the game controller
 **
 */

int RefereeGameController::getGCRobotID(int robotID) {
	// robots 1..11 are mapped to 0..10 in the game controller
	if (robotID > 0 && robotID <= MAX_NUM_PLAYERS)
		return robotID-1;

	// throw error on all other robots
	return -1;
}


/*------------------------------------------------------------------------------------------------*/

/**
 */

void RefereeGameController::handleRefereeMessage(RoboCupGameControlData *data) {
	if (data->version != STRUCT_VERSION) {
		refereeEnabled = false;
		ERROR("Referee game controller version mismatch (got %d, want %d)", data->version, STRUCT_VERSION);
		return;
	}

	// check whether we are an intended recipient
	int8_t teamIndex = -1;
	if (data->teams[0].teamNumber == teamID) {
		teamIndex = 0;
	} else if (data->teams[1].teamNumber == teamID) {
		teamIndex = 1;
	} else {
		// we are not one of the teams listed, ignore
		return;
	}

	WMOutput *cWM = wm.getCurrentWorldModel();

	// adjust our score
	if (data->teams[teamIndex].score != ourLastScore) {
		if (ourLastScore + 1 == data->teams[teamIndex].score && GCinitialized) {
			wm.setWeScored(true);
			//robot.switchBehavior(new BehaviorAnimation(M_SCORED), true);
		}
		ourLastScore = data->teams[teamIndex].score;
		INFO("We scored (%d:%d now)", ourLastScore, data->teams[1-teamIndex].score);
	}

	// adjust opponent score
	if (data->teams[1-teamIndex].score != oppLastScore) {
		if (oppLastScore + 1 == data->teams[1-teamIndex].score && GCinitialized) {
			wm.setOpponentScored(true);
		}

		oppLastScore = data->teams[1-teamIndex].score;
		INFO("Opponent scored (%d:%d now)", data->teams[teamIndex].score, oppLastScore);
	}

	// addendum to rules in RoboCup 2010 - on drop in, the robots position outside the circle and
	// the ball is in play right away
	/*if (data->kickOffTeam == 2) {
		if (data->state == STATE_PLAYING)
			data->kickOffTeam = teamIndex; // when playing, assume we have kickoff
		else
			data->kickOffTeam = 1 - teamIndex; // when not playing (ready/positioning), assume we are not having the kick off
	}*/

	// handle drop ball, alternative implementation
	if (data->kickOffTeam == 2 && cWM->kickOffMode != KICKOFF_DROPBALL) {
		INFO("Switching to dropball mode");
		wm.setKickOffMode(KICKOFF_DROPBALL);
		wm.setKickOffSide(KICKOFFSIDE_ANY);
	}

	// adjust kick off if our settings mismatch those of the referee
	if (    (data->kickOffTeam ==   teamIndex && cWM->kickOffSide != KICKOFFSIDE_ME)
	     || (data->kickOffTeam == 1-teamIndex && cWM->kickOffSide != KICKOFFSIDE_OPPONENT)
	   )
	{
		INFO("Switching kick off team to %s", data->kickOffTeam == teamIndex ? "us" : "opponent");
		wm.setKickOffMode(KICKOFF_REGULAR);
		wm.setKickOffSide(data->kickOffTeam == teamIndex ? KICKOFFSIDE_ME : KICKOFFSIDE_OPPONENT);
	}

	// adjust team color
	Color teamColor = data->teams[teamIndex].teamColour == TEAM_MAGENTA ? Magenta : Cyan;
	if (teamColor != cWM->teamColor) {
		INFO("Switching team color to %s", teamColor == Magenta ? "magenta" : "cyan");
		wm.setMyTeamColor(teamColor);
	}

	// adjust goal color
	Color myGoalColor = data->teams[teamIndex].goalColour == GOAL_YELLOW ? YellowGoal : BlueGoal;
	Color wmMyGoalColor = cWM->ourGoalColor;
	if (myGoalColor != wmMyGoalColor) {
		INFO("Switching my goal color to %s", myGoalColor == YellowGoal ? "yellow" : "blue");
		wm.setOurGoal(myGoalColor);
	}

	// adjust game state
	if (data->state == STATE_INITIAL && cWM->gameState != GAME_STOPPED) {
		INFO("Game state is now set to INITIAL");
		wm.setGameState(GAME_STOPPED);
	} else if (data->state == STATE_READY && cWM->gameState != GAME_READY) {
		INFO("Game state is now set to READY");
		wm.setGameState(GAME_READY);
	} else if (data->state == STATE_SET&& cWM->gameState != GAME_SET) {
		INFO("Game state is now set to SET");
		wm.setGameState(GAME_SET);
	} else if (data->state == STATE_PLAYING && cWM->gameState != GAME_STARTED) {
		INFO("Game state is now set to PLAY");
		wm.setGameState(GAME_STARTED);
	} else if (data->state == STATE_FINISHED&& cWM->gameState != GAME_STOPPED) {
		INFO("Game state is now set to FINISHED");
		wm.setGameState(GAME_STOPPED);
	}

	// penalty shootout?
	if (data->secGameState == STATE2_NORMAL && cWM->penaltyShoot) {
		INFO("Return to normal gameplay after penalty shoot mode");
		wm.setPenaltyShoot(false);
	} else if (data->secGameState == STATE2_PENALTYSHOOT && false == cWM->penaltyShoot) {
		INFO("Penalty shoot mode activated");
		wm.setPenaltyShoot(true);
	}
	int _gameState = (int)data->state;
	if (_gameState == 4)
		_gameState = 0;
	wm.setGameState((GameState)_gameState);

	// handle penalties
	int gcRobotID = getGCRobotID(robot.getID());
	if (gcRobotID != -1) {
		int remainingPenalizedTime = data->teams[teamIndex].players[ gcRobotID ].secsTillUnpenalised;
		if (remainingPenalizedTime > 0) {
			// turn on all lights when penalized
			if (cWM->penalized == false)
				motors.enableLED(MOTOR_ALL_MOTORS, true);

			wm.setPenalized(true);
			wm.setRemainingPenalizedTime(remainingPenalizedTime);

			// in the last few seconds blink the leds
			if (remainingPenalizedTime <= 5)
				motors.enableLED(MOTOR_ALL_MOTORS, !motors.isLEDEnabled(MOTOR_HEAD_ROLL));
		} else {
			// turn off all lights when unpenalized
			if (cWM->penalized == true)
				motors.enableLED(MOTOR_ALL_MOTORS, false);

			wm.setPenalized(false);
			wm.setRemainingPenalizedTime(0);
		}
	}

	GCinitialized = true;
}
