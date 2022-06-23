/**
 * @file GameInfo.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief the game information in RoboCup
 */

#include <iostream>
#include "GameData.h"

using namespace naoth;
using namespace std;

GameData::GameData()
  : 
    valid(false),
    playersPerTeam(0),

    competitionPhase(roundrobin),
    competitionType(competition_normal),
    gamePhase(normal),
    gameState(unknown_game_state),
    setPlay(set_none),

    firstHalf(true),
    kickingTeam(0),
    secsRemaining(0),
    secondaryTime(0),
    // HACK: for more info see declaration
    newPlayerNumber(0)
{
}

#define RETURN_VALUE_TO_STR(v) case v: return #v

std::string GameData::toString(TeamColor value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(blue);
    RETURN_VALUE_TO_STR(red);
    RETURN_VALUE_TO_STR(yellow);
    RETURN_VALUE_TO_STR(black);
    RETURN_VALUE_TO_STR(white);
    RETURN_VALUE_TO_STR(green);
    RETURN_VALUE_TO_STR(orange);
    RETURN_VALUE_TO_STR(purple);
    RETURN_VALUE_TO_STR(brown);
    RETURN_VALUE_TO_STR(gray);
    RETURN_VALUE_TO_STR(unknown_team_color);
  }

  ASSERT(false);
  return "invalid TeamColor";
}

std::string GameData::toString(CompetitionPhase value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(roundrobin);
    RETURN_VALUE_TO_STR(playoff);
  }
  
  ASSERT(false);
  return "invalid CompetitionPhase";
}

std::string GameData::toString(CompetitionType value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(competition_normal);
    RETURN_VALUE_TO_STR(COMPETITION_TYPE_CHALLENGE_SHIELD);
    RETURN_VALUE_TO_STR(COMPETITION_TYPE_7V7);
    RETURN_VALUE_TO_STR(COMPETITION_TYPE_DYNAMIC_BALL_HANDLING);
    //RETURN_VALUE_TO_STR(competition_1v1); // deprecated since 2022
    //RETURN_VALUE_TO_STR(competition_passing); // deprecated since 2022
    //RETURN_VALUE_TO_STR(competition_mixed); // deprecated since 2021
  }
  
  ASSERT(false);
  return "invalid CompetitionType";
}

std::string GameData::toString(GamePhase value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(normal);
    RETURN_VALUE_TO_STR(penaltyshoot);
    RETURN_VALUE_TO_STR(overtime);
    RETURN_VALUE_TO_STR(timeout);
  }
  
  ASSERT(false);
  return "invalid SecondaryGameState";
}

std::string GameData::toString(GameState value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(initial);
    RETURN_VALUE_TO_STR(ready);
    RETURN_VALUE_TO_STR(set);
    RETURN_VALUE_TO_STR(playing);
    RETURN_VALUE_TO_STR(finished);
    RETURN_VALUE_TO_STR(unknown_game_state);
  }
  
  ASSERT(false);
  return "invalid GameState";
}

std::string GameData::toString(SetPlay value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(set_none);
    RETURN_VALUE_TO_STR(goal_free_kick);
    RETURN_VALUE_TO_STR(pushing_free_kick);
    RETURN_VALUE_TO_STR(corner_kick);
    RETURN_VALUE_TO_STR(kick_in);
  }
  
  ASSERT(false);
  return "invalid SetPlay";
}


std::string GameData::toString(Penalty value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(penalty_none);
    RETURN_VALUE_TO_STR(illegal_ball_contact);
    RETURN_VALUE_TO_STR(player_pushing);
    RETURN_VALUE_TO_STR(illegal_motion_in_set);
    RETURN_VALUE_TO_STR(inactive_player);
    RETURN_VALUE_TO_STR(illegal_defender);
    RETURN_VALUE_TO_STR(leaving_the_field);
    RETURN_VALUE_TO_STR(kick_off_goal);
    RETURN_VALUE_TO_STR(request_for_pickup);
    RETURN_VALUE_TO_STR(local_game_stuck);
    RETURN_VALUE_TO_STR(illegal_positioning);
    RETURN_VALUE_TO_STR(substitute);
    RETURN_VALUE_TO_STR(manual);
  }
  
  ASSERT(false);
  return "invalid Penalty";
}

#define RETURN_STING_TO_VALUE(value, str) if(toString(value) == str) return value

GameData::TeamColor GameData::teamColorFromString(const std::string& str)
{
  RETURN_STING_TO_VALUE(blue, str);
  RETURN_STING_TO_VALUE(red, str);
  RETURN_STING_TO_VALUE(yellow, str);
  RETURN_STING_TO_VALUE(black, str);
  RETURN_STING_TO_VALUE(white, str);
  RETURN_STING_TO_VALUE(green, str);
  RETURN_STING_TO_VALUE(orange, str);
  RETURN_STING_TO_VALUE(purple, str);
  RETURN_STING_TO_VALUE(brown, str);
  RETURN_STING_TO_VALUE(gray, str);

  return unknown_team_color;
}

GameData::GameState GameData::gameStateFromString(const std::string& str)
{
  RETURN_STING_TO_VALUE(initial, str);
  RETURN_STING_TO_VALUE(ready, str);
  RETURN_STING_TO_VALUE(set, str);
  RETURN_STING_TO_VALUE(playing, str);
  RETURN_STING_TO_VALUE(finished, str);

  return unknown_game_state;
}

GameData::Penalty GameData::penaltyFromString(const std::string& str)
{
  RETURN_STING_TO_VALUE(penalty_none, str);
  RETURN_STING_TO_VALUE(illegal_ball_contact, str);
  RETURN_STING_TO_VALUE(player_pushing, str);
  RETURN_STING_TO_VALUE(illegal_motion_in_set, str);
  RETURN_STING_TO_VALUE(inactive_player, str);
  RETURN_STING_TO_VALUE(illegal_defender, str);
  RETURN_STING_TO_VALUE(leaving_the_field, str);
  RETURN_STING_TO_VALUE(kick_off_goal, str);
  RETURN_STING_TO_VALUE(request_for_pickup, str);
  RETURN_STING_TO_VALUE(local_game_stuck, str);
  RETURN_STING_TO_VALUE(illegal_positioning, str);
  RETURN_STING_TO_VALUE(substitute, str);
  RETURN_STING_TO_VALUE(manual, str);

  ASSERT(false);
  return manual;
}

void GameData::parseFrom(const spl::RoboCupGameControlData& data, int teamNumber)
{
  playersPerTeam    = data.playersPerTeam;

  competitionType   = (CompetitionType) data.competitionType;
  competitionPhase  = (CompetitionPhase) data.competitionPhase;
  gamePhase         = (GamePhase) data.gamePhase;
  gameState         = (GameState) data.state;
  setPlay           = (SetPlay) data.setPlay;

  firstHalf         = (data.firstHalf == 1);
  kickingTeam       = data.kickingTeam;

  // ACHTUNG: casting to signed values - game time can be negative (!)
  secsRemaining     = (int16_t)data.secsRemaining;
  secondaryTime     = (int16_t)data.secondaryTime;
  
  // team info
  if(data.teams[0].teamNumber == teamNumber) {
    parseTeamInfo(ownTeam, data.teams[0]);
    parseTeamInfo(oppTeam, data.teams[1]);
  } else if(data.teams[1].teamNumber == teamNumber) {
    parseTeamInfo(ownTeam, data.teams[1]);
    parseTeamInfo(oppTeam, data.teams[0]);
  } else {
    ASSERT(false);
  }
}

void GameData::parseTeamInfo(TeamInfo& teamInfoDst, const spl::TeamInfo& teamInfoSrc) const
{
  teamInfoDst.penaltyShot = teamInfoSrc.penaltyShot;
  teamInfoDst.score = teamInfoSrc.score;
  teamInfoDst.teamColor = (TeamColor)teamInfoSrc.teamColour;
  teamInfoDst.teamNumber = teamInfoSrc.teamNumber;

  teamInfoDst.players.resize(playersPerTeam);
  for(unsigned int i = 0; i < playersPerTeam; i++) {
    teamInfoDst.players[i].penalty = (Penalty)teamInfoSrc.players[i].penalty;

    // ACHTUNG: casting to signed values - time can be negative (!)
    teamInfoDst.players[i].secsTillUnpenalised = (int8_t)teamInfoSrc.players[i].secsTillUnpenalised;
  }
}


void GameData::print(ostream& stream) const
{
  stream << "playersPerTeam = " << playersPerTeam << std::endl;
  
  stream << "competitionPhase = " << toString(competitionPhase) << std::endl;
  stream << "competitionType = "  << toString(competitionType) << std::endl;
  stream << "gamePhase = "        << toString(gamePhase) << std::endl;
  stream << "gameState = "        << toString(gameState) << std::endl;
  stream << "setPlay = "          << toString(setPlay) << std::endl;

  stream << "firstHalf = "      << firstHalf << std::endl;
  stream << "kickingTeam = "    << kickingTeam << std::endl;
  stream << "secsRemaining = "  << secsRemaining << std::endl;
  stream << "secondaryTime = "  << secondaryTime << std::endl;

  stream << std::endl;
  stream << "Own Team:" << std::endl;
  stream << " |- number = " << ownTeam.teamNumber << std::endl;
  stream << " |- color = " << toString(ownTeam.teamColor) << std::endl;
  stream << " |- score = " << ownTeam.score << std::endl;
  stream << " |- penaltyShot = " << ownTeam.penaltyShot << std::endl;
  stream << " |- players (penalty, time until unpenalize in s):" << std::endl;
  for(size_t i = 0; i < ownTeam.players.size(); ++i) {
    stream << "      |- " << (i+1) << ": " << toString(ownTeam.players[i].penalty) << " - " << ownTeam.players[i].secsTillUnpenalised << std::endl;
  }

  stream << std::endl;
  stream << "Opp Team:" << std::endl;
  stream << " |- number = " << oppTeam.teamNumber << std::endl;
  stream << " |- color = " << toString(oppTeam.teamColor) << std::endl;
  stream << " |- score = " << oppTeam.score << std::endl;
  stream << " |- penaltyShot = " << oppTeam.penaltyShot << std::endl;
  stream << " |- players (penalty, time until unpenalize in s):" << std::endl;
  for(size_t i = 0; i < oppTeam.players.size(); ++i) {
    stream << "      |- " << (i+1) << ": " << toString(oppTeam.players[i].penalty) << " - " << oppTeam.players[i].secsTillUnpenalised << std::endl;
  }
}


std::string GameReturnData::toString(FallenState value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(ROBOT_CAN_PLAY);
    RETURN_VALUE_TO_STR(ROBOT_FALLEN);
  }
  
  ASSERT(false);
  return "invalide Message";
}



