/**
 * @file GameInfo.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 *
 * @breief This representation reflects the game status as communicated by the game controller. 
 *         Essentially this class is a C++ version of RoboCupGameControlData.
 */

#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <string>
#include <vector>

#include "Tools/DataStructures/Printable.h"
#include "Representations/Infrastructure/Configuration.h"
#include <MessagesSPL/RoboCupGameControlData.h>
#include <Tools/Debug/NaoTHAssert.h>


namespace naoth
{

class GameData : public Printable
{
public:

  enum TeamColor
  {
    blue    = TEAM_BLUE,
    red     = TEAM_RED,
    yellow  = TEAM_YELLOW,
    black   = TEAM_BLACK,
    unknown_team_color
  };

  enum GameType
  {
    roundrobin = GAME_ROUNDROBIN,
    playoff    = GAME_PLAYOFF,
    dropin     = GAME_DROPIN
  };

  enum GameState
  {
    initial   = STATE_INITIAL,
    ready     = STATE_READY,
    set       = STATE_SET,
    playing   = STATE_PLAYING,
    finished  = STATE_FINISHED,
    unknown_game_state
  };

  enum SecondaryGameState
  {
    normal        = STATE2_NORMAL,
    penaltyshoot  = STATE2_PENALTYSHOOT,
    overtime      = STATE2_OVERTIME,
    timeout       = STATE2_TIMEOUT
  };

  enum PenaltyState
  {
    none                  = PENALTY_NONE,
    illegal_ball_contact  = PENALTY_SPL_ILLEGAL_BALL_CONTACT,
    player_pushing        = PENALTY_SPL_PLAYER_PUSHING,
    illegal_motion_in_set = PENALTY_SPL_ILLEGAL_MOTION_IN_SET,
    inactive_player       = PENALTY_SPL_INACTIVE_PLAYER,
    illegal_defender      = PENALTY_SPL_ILLEGAL_DEFENDER,
    leaving_the_field     = PENALTY_SPL_LEAVING_THE_FIELD,
    kick_off_goal         = PENALTY_SPL_KICK_OFF_GOAL,
    request_for_pickup    = PENALTY_SPL_REQUEST_FOR_PICKUP,
    coach_motion          = PENALTY_SPL_COACH_MOTION,
    substitute            = PENALTY_SUBSTITUTE,
    manual                = PENALTY_MANUAL
  };


  struct RobotInfo
  {
    RobotInfo() : penalty(none), secsTillUnpenalised(0) {}
    PenaltyState penalty;             // penalty state of the player

    // ACHTUNG: time can be negative (!)
    int secsTillUnpenalised;          // estimate of time till unpenalised

    bool isPenalized() const { return penalty != none; }
  };

  struct TeamInfo
  {
    TeamInfo() :
      teamNumber(0),
      teamColour(unknown_team_color),
      score(0),
      penaltyShot(0)
    {}

    unsigned int teamNumber;        // unique team number
    TeamColor teamColour;           // colour of the team
    unsigned int  score;            // team's score
    unsigned int  penaltyShot;      // penalty shot counter
    std::vector<RobotInfo> players; // the team's players

    // NOTE: not used yet
    //uint16_t singleShots;     // bits represent penalty shot success
    //uint8_t coachSequence;    // sequence number of the coach's message
    //uint8_t coachMessage[SPL_COACH_MESSAGE_SIZE]; // the coach's message to the team
    //RobotInfo coach;
  };

  static std::string toString(TeamColor value);
  static std::string toString(GameType value);
  static std::string toString(GameState value);
  static std::string toString(SecondaryGameState value);
  static std::string toString(PenaltyState value);

  static TeamColor teamColorFromString(const std::string& str);
  static GameState gameStateFromString(const std::string& str);

  virtual void print(std::ostream& stream) const;
  void parseFrom(const spl::RoboCupGameControlData& data, int teamNumber);
  void parseTeamInfo(TeamInfo& teamInfoDst, const spl::TeamInfo& teamInfoSrc) const;

  GameData();

  const RobotInfo& getOwnRobotInfo(int playerNumber) const {
    ASSERT(playerNumber > 0 && playerNumber <= (int)(ownTeam.players.size()));
    return ownTeam.players[playerNumber-1];
  }


public:
  bool valid; // indicates that this represenation was filled

  int playersPerTeam;                 // the number of players on a team
  GameType gameType;                  // type of the game (GAME_ROUNDROBIN, GAME_PLAYOFF, GAME_DROPIN)
  GameState gameState;                // state of the game (STATE_READY, STATE_PLAYING, etc)
  bool firstHalf;                     // 1 = game in first half, 0 otherwise
  unsigned int kickOffTeam;           // the team number of the next team to kick off or DROPBALL
  SecondaryGameState secondaryState;  // extra state information - (STATE2_NORMAL, STATE2_PENALTYSHOOT, etc)
  unsigned int dropInTeam;            // number of team that caused last drop in

  // ACHTUNG: time and can be negative, so it has to be signed (!)
  int dropInTime;                     // number of seconds passed since the last drop in. -1 (0xffff) before first dropin
  int secsRemaining;                  // estimate of number of seconds remaining in the half
  int secondaryTime;                  // number of seconds shown as secondary time (remaining ready, until free ball, etc)
  
  TeamInfo ownTeam;
  TeamInfo oppTeam;

  // HACK: thi is only provided by SimSpark - find a better solution
  // if this is set to anything other than 0, the actual player number will change
  unsigned int newPlayerNumber;
};

class GameReturnData: public Printable
{
public:
  GameReturnData()
    :
    team(0),
    player(0),
    message(alive)
  {}

  enum Message
  {
    manual_penalise   = GAMECONTROLLER_RETURN_MSG_MAN_PENALISE,
    manual_unpenalise = GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE,
    alive             = GAMECONTROLLER_RETURN_MSG_ALIVE
  };

  static std::string toString(Message value);

  int team;
  int player;
  Message message;

  virtual void print(std::ostream& stream) const
  {
    stream << "team:\t"     << team << std::endl;
    stream << "player:\t"   << player << std::endl;
    stream << "message:\t"  << toString(message) << std::endl;
  }
};

}// namespace naoth

#endif // GAMEDATA_H
