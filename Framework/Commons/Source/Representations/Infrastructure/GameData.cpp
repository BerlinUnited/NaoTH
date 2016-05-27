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
    gameType(roundrobin),
    gameState(unknown_game_state),
    firstHalf(true),
    kickOffTeam(-1),
    secondaryState(normal),
    dropInTeam(-1),
    dropInTime(0),
    secsRemaining(0),
    secondaryTime(0)
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
    RETURN_VALUE_TO_STR(unknown_team_color);
  }

  ASSERT(false);
  return "invalide TeamColor";
}

std::string GameData::toString(GameType value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(roundrobin);
    RETURN_VALUE_TO_STR(playoff);
    RETURN_VALUE_TO_STR(dropin);
  }
  
  ASSERT(false);
  return "invalide GameType";
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
  return "invalide GameState";
}

std::string GameData::toString(SecondaryGameState value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(normal);
    RETURN_VALUE_TO_STR(penaltyshoot);
    RETURN_VALUE_TO_STR(overtime);
    RETURN_VALUE_TO_STR(timeout);
  }
  
  ASSERT(false);
  return "invalide SecondaryGameState";
}

std::string GameData::toString(PenaltyState value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(none);
    RETURN_VALUE_TO_STR(illegal_ball_contact);
    RETURN_VALUE_TO_STR(player_pushing);
    RETURN_VALUE_TO_STR(illegal_motion_in_set);
    RETURN_VALUE_TO_STR(inactive_player);
    RETURN_VALUE_TO_STR(illegal_defender);
    RETURN_VALUE_TO_STR(leaving_the_field);
    RETURN_VALUE_TO_STR(kick_off_goal);
    RETURN_VALUE_TO_STR(request_for_pickup);
    RETURN_VALUE_TO_STR(coach_motion);
    RETURN_VALUE_TO_STR(substitute);
    RETURN_VALUE_TO_STR(manual);
  }
  
  ASSERT(false);
  return "invalide PenaltyState";
}

#define RETURN_STING_TO_VALUE(value, str) if(toString(value) == str) return value

GameData::TeamColor GameData::teamColorFromString(const std::string& str)
{
  RETURN_STING_TO_VALUE(blue, str);
  RETURN_STING_TO_VALUE(red, str);
  RETURN_STING_TO_VALUE(yellow, str);
  RETURN_STING_TO_VALUE(black, str);

  return unknown_team_color;
}

void GameData::parseFrom(const spl::RoboCupGameControlData& data, int teamNumber)
{
  playersPerTeam  = data.playersPerTeam;
  gameType        = (GameType)data.gameType;
  gameState       = (GameState)data.state;
  firstHalf       = data.firstHalf == 1;
  kickOffTeam     = data.kickOffTeam;
  secondaryState  = (SecondaryGameState)data.secondaryState;
  dropInTeam      = data.dropInTeam;

  // ACHTUNG: casting to signed values - game time can be negative (!)
  dropInTime      = (int16_t)data.dropInTime;
  secsRemaining   = (int16_t)data.secsRemaining;
  secondaryTime   = (int16_t)data.secondaryTime;
  
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
  teamInfoDst.teamColour = (TeamColor)teamInfoSrc.teamColour;
  teamInfoDst.teamNumber = teamInfoSrc.teamNumber;

  teamInfoDst.players.resize(playersPerTeam);
  for(int i = 0; i < playersPerTeam; i++) {
    teamInfoDst.players[i].penalty = (PenaltyState)teamInfoSrc.players[i].penalty;
    teamInfoDst.players[i].secsTillUnpenalised = teamInfoSrc.players[i].secsTillUnpenalised;
  }
}


void GameData::print(ostream& stream) const
{
  stream << "playersPerTeam = " << playersPerTeam << std::endl;
  stream << "gameType = "       << toString(gameType) << std::endl;
  stream << "state = "          << toString(gameState) << std::endl;
  stream << "firstHalf = "      << firstHalf << std::endl;
  stream << "kickOffTeam = "    << kickOffTeam << std::endl;
  stream << "secondaryState = " << toString(secondaryState) << std::endl;
  stream << "dropInTeam = "     << dropInTeam << std::endl;
  stream << "dropInTime = "     << dropInTime << std::endl;
  stream << "secsRemaining = "  << secsRemaining << std::endl;
  stream << "secondaryTime = "  << secondaryTime << std::endl;

  stream << std::endl;
  stream << "Own Team:" << std::endl;
  stream << " |- number = " << ownTeam.teamNumber << std::endl;
  stream << " |- color = " << toString(ownTeam.teamColour) << std::endl;
  stream << " |- score = " << ownTeam.score << std::endl;
  stream << " |- penaltyShot = " << ownTeam.penaltyShot << std::endl;
  stream << " |- players (penalty, time until unpenalize in s):" << std::endl;
  for(size_t i = 0; i < ownTeam.players.size(); ++i) {
    stream << "      |- " << (i+1) << ": " << toString(ownTeam.players[i].penalty) << " - " << ownTeam.players[i].secsTillUnpenalised << std::endl;
  }

  stream << std::endl;
  stream << "Opp Team:" << std::endl;
  stream << " |- number = " << oppTeam.teamNumber << std::endl;
  stream << " |- color = " << toString(oppTeam.teamColour) << std::endl;
  stream << " |- score = " << oppTeam.score << std::endl;
  stream << " |- penaltyShot = " << oppTeam.penaltyShot << std::endl;
  stream << " |- players (penalty, time until unpenalize in s):" << std::endl;
  for(size_t i = 0; i < oppTeam.players.size(); ++i) {
    stream << "      |- " << (i+1) << ": " << toString(oppTeam.players[i].penalty) << " - " << oppTeam.players[i].secsTillUnpenalised << std::endl;
  }
}


std::string GameReturnData::toString(Message value)
{
  switch (value)
  {
    RETURN_VALUE_TO_STR(manual_penalise);
    RETURN_VALUE_TO_STR(manual_unpenalise);
    RETURN_VALUE_TO_STR(alive);
  }
  
  ASSERT(false);
  return "invalide Message";
}



