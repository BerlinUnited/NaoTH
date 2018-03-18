#ifndef ROBOCUPGAMECONTROLDATA_H
#define ROBOCUPGAMECONTROLDATA_H

#include <stdint.h>

#define GAMECONTROLLER_DATA_PORT       3838
#define GAMECONTROLLER_RETURN_PORT     3939

#define GAMECONTROLLER_STRUCT_HEADER   "RGme"
#define GAMECONTROLLER_STRUCT_VERSION  11

#define MAX_NUM_PLAYERS             6

// SPL
#define TEAM_BLUE                   0 // cyan, blue, violet
#define TEAM_RED                    1 // magenta, pink (not red/orange)
#define TEAM_YELLOW                 2 // yellow
#define TEAM_BLACK                  3 // black, dark gray
#define TEAM_WHITE                  4 // white
#define TEAM_GREEN                  5 // green
#define TEAM_ORANGE                 6 // orange
#define TEAM_PURPLE                 7 // purple, violet
#define TEAM_BROWN                  8 // brown
#define TEAM_GRAY                   9 // lighter grey

// HL
#define TEAM_CYAN                   0
#define TEAM_MAGENTA                1
#define DROPBALL                    255

#define COMPETITION_PHASE_ROUNDROBIN        0
#define COMPETITION_PHASE_PLAYOFF           1

#define COMPETITION_TYPE_NORMAL               0
#define COMPETITION_TYPE_MIXEDTEAM            1
#define COMPETITION_TYPE_GENERAL_PENALTY_KICK 2

#define GAME_PHASE_NORMAL                   0
#define GAME_PHASE_PENALTYSHOOT             1
#define GAME_PHASE_OVERTIME                 2
#define GAME_PHASE_TIMEOUT                  3

#define STATE_INITIAL                       0
#define STATE_READY                         1
#define STATE_SET                           2
#define STATE_PLAYING                       3
#define STATE_FINISHED                      4

#define SET_PLAY_NONE                       0
#define SET_PLAY_GOAL_FREE_KICK             1
#define SET_PLAY_PUSHING_FREE_KICK          2

#define PENALTY_NONE                        0
// SPL
#define PENALTY_SPL_ILLEGAL_BALL_CONTACT    1 // ball holding / playing with hands
#define PENALTY_SPL_PLAYER_PUSHING          2
#define PENALTY_SPL_ILLEGAL_MOTION_IN_SET   3 // heard whistle too early?
#define PENALTY_SPL_INACTIVE_PLAYER         4 // fallen, inactive
#define PENALTY_SPL_ILLEGAL_DEFENDER        5 // own penalty area, center circle during kick-off
#define PENALTY_SPL_LEAVING_THE_FIELD       6
#define PENALTY_SPL_KICK_OFF_GOAL           7 // scored from inside center circle after kick-off
#define PENALTY_SPL_REQUEST_FOR_PICKUP      8
#define PENALTY_SPL_LOCAL_GAME_STUCK        9
// HL Kid Size
#define PENALTY_HL_KID_BALL_MANIPULATION    1
#define PENALTY_HL_KID_PHYSICAL_CONTACT     2
#define PENALTY_HL_KID_ILLEGAL_ATTACK       3
#define PENALTY_HL_KID_ILLEGAL_DEFENSE      4
#define PENALTY_HL_KID_REQUEST_FOR_PICKUP   5
#define PENALTY_HL_KID_REQUEST_FOR_SERVICE  6
#define PENALTY_HL_KID_REQUEST_FOR_PICKUP_2_SERVICE 7
// HL Teen Size
#define PENALTY_HL_TEEN_BALL_MANIPULATION   1
#define PENALTY_HL_TEEN_PHYSICAL_CONTACT    2
#define PENALTY_HL_TEEN_ILLEGAL_ATTACK      3
#define PENALTY_HL_TEEN_ILLEGAL_DEFENSE     4
#define PENALTY_HL_TEEN_REQUEST_FOR_PICKUP  5
#define PENALTY_HL_TEEN_REQUEST_FOR_SERVICE 6
#define PENALTY_HL_TEEN_REQUEST_FOR_PICKUP_2_SERVICE 7

#define PENALTY_SUBSTITUTE                  14
#define PENALTY_MANUAL                      15

// NOTE: added by Heinrich
namespace spl
{

struct RobotInfo
{
  uint8_t penalty;              // penalty state of the player
  uint8_t secsTillUnpenalised;  // estimate of time till unpenalised
};

struct TeamInfo
{
  uint8_t teamNumber;           // unique team number
  uint8_t teamColor;            // color of the team
  uint8_t score;                // team's score
  uint8_t penaltyShot;          // penalty shot counter
  uint16_t singleShots;         // bits represent penalty shot success
  RobotInfo players[MAX_NUM_PLAYERS]; // the team's players
};

struct RoboCupGameControlData
{
  char header[4];               // header to identify the structure
  uint16_t version;             // version of the data structure
  uint8_t packetNumber;         // number incremented with each packet sent (with wraparound)
  uint8_t playersPerTeam;       // the number of players on a team
  uint8_t competitionPhase;     // phase of the competition (COMPETITION_PHASE_ROUNDROBIN, COMPETITION_PHASE_PLAYOFF)
  uint8_t competitionType;      // type of the competition (COMPETITION_TYPE_NORMAL, COMPETITION_TYPE_MIXEDTEAM, COMPETITION_TYPE_GENERAL_PENALTY_KICK)
  uint8_t gamePhase;            // phase of the game (GAME_PHASE_NORMAL, GAME_PHASE_PENALTYSHOOT, etc)
  uint8_t state;                // state of the game (STATE_READY, STATE_PLAYING, etc)
  uint8_t setPlay;              // active set play (SET_PLAY_NONE, SET_PLAY_GOAL_FREE_KICK, etc)
  uint8_t firstHalf;            // 1 = game in first half, 0 otherwise
  uint8_t kickingTeam;          // the team number of the next team to kick off, free kick, DROPBALL etc.
  uint8_t dropInTeam;           // number of team that caused last drop in
  uint16_t dropInTime;          // number of seconds passed since the last drop in. -1 (0xffff) before first dropin
  uint16_t secsRemaining;       // estimate of number of seconds remaining in the half
  uint16_t secondaryTime;       // number of seconds shown as secondary time (remaining ready, until free ball, etc)
  TeamInfo teams[2];
};

// data structure header
#define GAMECONTROLLER_RETURN_STRUCT_HEADER      "RGrt"
#define GAMECONTROLLER_RETURN_STRUCT_VERSION     2

#define GAMECONTROLLER_RETURN_MSG_MAN_PENALISE   0
#define GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE 1
#define GAMECONTROLLER_RETURN_MSG_ALIVE          2

struct RoboCupGameControlReturnData
{
  char header[4];
  uint8_t version;
  uint8_t team;    // team number
  uint8_t player;  // player number starts with 1
  uint8_t message; // one of the three messages defined above

#ifdef __cplusplus
  // constructor
  RoboCupGameControlReturnData() : version(GAMECONTROLLER_RETURN_STRUCT_VERSION)
  {
    const char* init = GAMECONTROLLER_RETURN_STRUCT_HEADER;
    for(unsigned int i = 0; i < sizeof(header); ++i)
      header[i] = init[i];
  }
#endif
};
} // namespace spl

#endif // ROBOCUPGAMECONTROLDATA_H
