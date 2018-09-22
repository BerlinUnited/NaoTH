/**
 * @file: TeamSymbols.cpp
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First created on 9. April 2009, 18:10
 */

#include "TeamSymbols.h"

TeamSymbols* TeamSymbols::theInstance = NULL;

void TeamSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("team.members_alive_count", &getTeamMembersAliveCount);
  engine.registerDecimalInputSymbol("team.members_active_count", &getTeamMembersActiveCount);
  engine.registerBooleanInputSymbol("team.calc_if_is_striker", &calculateIfStriker);
  engine.registerBooleanInputSymbol("team.calc_if_is_secondstriker", &calculateIfSecondStriker);
  engine.registerBooleanOutputSymbol("team.is_playing_as_striker",&setWasStriker, &getWasStriker);
}

double TeamSymbols::getTeamMembersAliveCount()
{
  return (double) theInstance->getTeamMessagePlayerIsAlive().getAliveCount();
}//end getTeamMembersAliveCount

double TeamSymbols::getTeamMembersActiveCount()
{
  return (double) theInstance->getTeamMessagePlayerIsAlive().getActiveCount();
}//end getTeamMembersActiveCount

bool TeamSymbols::getWasStriker()
{
  return theInstance->getPlayerInfo().isPlayingStriker;
}

void TeamSymbols::setWasStriker(bool striker)
{
  theInstance->getPlayerInfo().isPlayingStriker = striker;

  // priorize the decision to become a striker by the goalie
  if (striker && theInstance->getPlayerInfo().playerNumber == 1) {
    theInstance->getRoleDecisionModel().wantsToBeStriker = true;
  }
}

bool TeamSymbols::calculateIfStriker()
{
  return theInstance->getRoleDecisionModel().firstStriker == theInstance->getPlayerInfo().playerNumber;
}

bool TeamSymbols::calculateIfSecondStriker()
{
  return theInstance->getRoleDecisionModel().secondStriker == theInstance->getPlayerInfo().playerNumber;
}
