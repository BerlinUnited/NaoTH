/** @file
 **
 ** Communication interface to old robots
**/

#ifndef __REFEREE_H__
#define __REFEREE_H__

#include "system/timer.h"
#include "system/thread.h"
#include "platform/generic/singleton.h"

#include "transport/transport_udp.h"

#include <arpa/inet.h>


/*------------------------------------------------------------------------------------------------*/

#define DEFAULTTEAMID 99

/*------------------------------------------------------------------------------------------------*/

#define DEFAULTREFEREEENABLED  1

/*------------------------------------------------------------------------------------------------*/

#define REFEREEPORT 3838

/*------------------------------------------------------------------------------------------------*/


// the following definitions taken from
// http://robocupgc.svn.sourceforge.net/viewvc/robocupgc/trunk/doc/index.html

#define STRUCT_VERSION              7

// data structure header
#define STRUCT_HEADER               "RGme"

#define MAX_NUM_PLAYERS             11

// team numbers
#define TEAM_BLUE                   0
#define TEAM_CYAN                   0
#define TEAM_RED                    1
#define TEAM_MAGENTA                1

#define GOAL_BLUE                   0
#define GOAL_YELLOW                 1

// game states
#define STATE_INITIAL               0
#define STATE_READY                 1
#define STATE_SET                   2
#define STATE_PLAYING               3
#define STATE_FINISHED              4

#define STATE2_NORMAL               0
#define STATE2_PENALTYSHOOT         1

// penalties
#define PENALTY_NONE                0

// Information that describes a player.
struct RobotInfo {
	uint16_t penalty;             // penalty state of the player, reserved for future use
	uint16_t secsTillUnpenalised; // estimate of time till unpenalised
};


// Information that describes a team.
struct TeamInfo {
	uint8_t   teamNumber;          // unique team number
	uint8_t   teamColour;          // colour of the team
	uint8_t   goalColour;          // colour of the goal
	uint8_t   score;               // team's score

	RobotInfo players[MAX_NUM_PLAYERS];  // the team's players
};

// Information that describes the game state.
struct RoboCupGameControlData {
	char     header[4];           // header to identify the structure
	uint32_t version;             // version of the data structure

	uint8_t  playersPerTeam;      // number of players on a team
	uint8_t  state;               // state of the game
	uint8_t  firstHalf;           // 1 = game in first half, 0 otherwise
	uint8_t  kickOffTeam;         // the next team to kick off

	uint8_t  secGameState;        // extra state information - (STATE2_NORMAL, STATE2_PENALTYSHOOT, etc)
	uint8_t  dropInTeam;          // team that caused last drop in
	uint16_t dropInTime;          // number of seconds passed since the last drop in.  -1 before first dropin

	uint32_t secsRemaining;       // estimate of number of seconds remaining in the half

	TeamInfo teams[2];            // team information
};

// end of definitions from
// http://robocupgc.svn.sourceforge.net/viewvc/robocupgc/trunk/doc/index.html


/*------------------------------------------------------------------------------------------------*/

class RefereeGameController : public Singleton<RefereeGameController>, public Thread {
private:
	/// constructor (protected, this is a singleton)
	RefereeGameController();

	friend class Singleton<RefereeGameController>;

protected:
	/// UDP transport
	TransportUDP *transport;

	/// our global team ID
	int teamID;

	/// whether referee commands are processed
	bool refereeEnabled;

	uint8_t     ourLastScore;
	uint8_t     oppLastScore;

	/// critical section
	CriticalSection cs;

	/// thread main function, waits for incoming packets
	void threadMain();
	bool GCinitialized;

	void handleRefereeMessage(RoboCupGameControlData *data);
	int getGCRobotID(int robotID);

  public:
	virtual ~RefereeGameController();

	/// get a descriptive name of the thread
	virtual const char* getName() {
		return "RefereeGameController";
	}

	void init();
};

#endif
