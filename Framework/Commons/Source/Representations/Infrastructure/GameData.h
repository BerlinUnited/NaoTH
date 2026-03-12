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
#include <map>

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"

#include "Tools/DataStructures/Printable.h"
#include <MessagesSPL/RoboCupGameControlData.h>
#include <Tools/Debug/NaoTHAssert.h>


namespace naoth
{

class GameData : public Printable
{
public:

  enum TeamColor
  {
    blue   = TEAM_BLUE,   // blue, cyan
    red    = TEAM_RED,    // red, magenta, pink
    yellow = TEAM_YELLOW, // yellow
    black  = TEAM_BLACK,  // black, dark gray

    white  = TEAM_WHITE,  // white
    green  = TEAM_GREEN,  // green
    orange = TEAM_ORANGE, // orange
    purple = TEAM_PURPLE, // purple, violet
    brown  = TEAM_BROWN,  // brown
    gray   = TEAM_GRAY,   // lighter gray
    
    // invalide team color value
    unknown_team_color = 255
  };

  enum CompetitionType
  {
    competition_type_small   = COMPETITION_TYPE_SMALL,
    competition_type_middle   = COMPETITION_TYPE_MIDDLE,
    competition_type_large   = COMPETITION_TYPE_LARGE,
  };

  enum GamePhase
  {
    normal        = GAME_PHASE_NORMAL,
    penaltyshoot  = GAME_PHASE_PENALTYSHOOT,
    overtime      = GAME_PHASE_EXTRATIME,
    timeout       = GAME_PHASE_TIMEOUT
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


  enum SetPlay 
  {
    set_none           = SET_PLAY_NONE,
    direct_free_kick   = SET_PLAY_DIRECT_FREE_KICK,
    indirect_free_kick = SET_PLAY_INDIRECT_FREE_KICK,
    penalty_kick       = SET_PLAY_PENALTY_KICK,
    throw_in           = SET_PLAY_THROW_IN,
    goal_kick          = SET_PLAY_GOAL_KICK,
    corner_kick        = SET_PLAY_CORNER_KICK  
  };


  enum Penalty
  {
    penalty_none              = PENALTY_NONE,

    illegal_positioning       = PENALTY_ILLEGAL_POSITIONING,
    motion_in_set             = PENALTY_MOTION_IN_SET,
    local_game_stuck          = PENALTY_LOCAL_GAME_STUCK,
    incapable_robot           = PENALTY_INCAPABLE_ROBOT,
    pickup                    = PENALTY_PICK_UP,
    ball_holding              = PENALTY_BALL_HOLDING,
    leaving_the_field         = PENALTY_LEAVING_THE_FIELD,
    playing_with_arms_hands   = PENALTY_PLAYING_WITH_ARMS_HANDS,
    pushing                   = PENALTY_PUSHING,
    sent_off                  = PENALTY_SENT_OFF,
    substitute                = PENALTY_SUBSTITUTE,
    manual,
  };


  struct RobotInfo
  {
    RobotInfo() : penalty(penalty_none), secsTillUnpenalised(0) {}
    Penalty penalty;             // penalty state of the player

    // ACHTUNG: time can be negative (!)
    int secsTillUnpenalised;          // estimate of time till unpenalised

    bool isPenalized() const { return penalty != penalty_none; }
  };

  struct TeamInfo
  {
    TeamInfo() :
      teamNumber(0),
      teamColor(unknown_team_color),
      score(0),
      penaltyShot(0),
      messageBudget(1200)
    {}

    unsigned int teamNumber;             // unique team number
    TeamColor teamColor;                 // colour of the team
    unsigned int score;                  // team's score
    unsigned int penaltyShot;            // penalty shot counter
    unsigned int messageBudget;          // number of team messages the team is allowed to send for the remainder of the game
    std::map<size_t, struct RobotInfo> players; // the team's players

    // NOTE: not used yet
    //unsigned int goalieNumber;      // unique goalie number
    //TeamColor goalieColor;          // colour of the goalie
    //uint16_t singleShots;     // bits represent penalty shot success
  };

  void updateSetPlayState();

  static std::string toString(TeamColor value);
  static std::string toString(CompetitionType value);
  static std::string toString(GamePhase value);
  static std::string toString(GameState value);
  static std::string toString(SetPlay value);
  static std::string toString(Penalty value);

  static TeamColor teamColorFromString(const std::string& str);
  static GameState gameStateFromString(const std::string& str);
  static Penalty penaltyFromString(const std::string& str);

  virtual void print(std::ostream& stream) const;
  void parseFrom(const hsl::RoboCupGameControlData& data, int teamNumber);
  void parseTeamInfo(TeamInfo& teamInfoDst, const hsl::TeamInfo& teamInfoSrc) const;

  GameData();

  const RobotInfo& getOwnRobotInfo(size_t playerNumber) const {
    // If not an active Player, abort
    ASSERT(isActivePlayer(playerNumber));
    return ownTeam.players.at(playerNumber);
  }

  inline bool isActivePlayer(size_t playerNumber) const {
    return ownTeam.players.count(playerNumber) > 0;
  }


public:
  bool valid; // indicates that this represenation was filled

  unsigned int playersPerTeam;        // the number of players on a team

  CompetitionType  competitionType;   // type of the competition (COMPETITION_TYPE_NORMAL, COMPETITION_TYPE_1VS1_CHALLENGE, COMPETITION_TYPE_PASSING_CHALLENGE)
  GamePhase        gamePhase;         // phase of the game (GAME_PHASE_NORMAL, GAME_PHASE_PENALTYSHOOT, etc)
  GameState        gameState;         // state of the game (STATE_READY, STATE_PLAYING, etc)
  SetPlay          setPlay;           // active set play (SET_PLAY_NONE, SET_PLAY_GOAL_FREE_KICK, etc)

  bool firstHalf;                     // 1 = game in first half, 0 otherwise
  unsigned int kickingTeam;           // the team number of the next team to kick off, free kick, DROPBALL etc.

  int secsRemaining;                  // estimate of number of seconds remaining in the half
  int secondaryTime;                  // number of seconds shown as secondary time (remaining ready, until free ball, etc)
  
  TeamInfo ownTeam;
  TeamInfo oppTeam;


  bool stopped; // Emergency stop that is not part of any state, but a global flag.

  // HACK: this is only provided by SimSpark - find a better solution
  // if this is set to anything other than 0, the actual player number will change
  unsigned int newPlayerNumber;
};

class GameReturnData: public Printable
{
public:
  GameReturnData()
    :
    playerNum(0),
    teamNum(0),
    fallen(ROBOT_CAN_PLAY),
    ballAge(-1)
  {}

  enum FallenState
  {
    ROBOT_CAN_PLAY    = 0,
    ROBOT_FALLEN      = 1
  };

  static std::string toString(FallenState value);

  int playerNum;
  int teamNum;
  
  FallenState fallen;    // 1 means that the robot is fallen, 0 means that the robot can play

  Pose2D pose;           // robot pose
  double ballAge;        // milliseconds since this robot last saw the ball. -1 if we haven't seen it
  Vector2d ballPosition; // position of ball relative to the robot coordinates in millimeters

  // wrote the values to the standard SPL message
  void writeTo(hsl::RoboCupGameControlReturnData& data) const;

  virtual void print(std::ostream& stream) const
  {
    stream << "player:  " << playerNum << std::endl;
    stream << "team:    " << teamNum << std::endl;
    stream << "fallen:  " << toString(fallen) << std::endl;
    stream << "pose:    " << pose << std::endl;
    stream << "ballAge: " << ballAge << std::endl;
    stream << "ball:    " << ballPosition << std::endl;
  }
};

}// namespace naoth

#endif // GAMEDATA_H
