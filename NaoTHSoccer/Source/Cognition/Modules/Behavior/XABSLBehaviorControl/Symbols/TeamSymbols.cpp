/**
 * @file: TeamSymbols.cpp
 * @author: <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 *
 * First created on 9. April 2009, 18:10
 */

#include "TeamSymbols.h"

void TeamSymbols::registerSymbols(xabsl::Engine& engine)
{
  engine.registerDecimalInputSymbol("team.members_alive_count", &getTeamMembersAliveCount);
  engine.registerBooleanInputSymbol("team.calc_if_is_striker", &calculateIfStriker);
  engine.registerBooleanInputSymbol("team.calc_if_is_secondstriker", &calculateIfSecondStriker);
  engine.registerBooleanOutputSymbol("team.is_playing_as_striker",&setWasStriker, &getWasStriker);
}


TeamSymbols* TeamSymbols::theInstance = NULL;

void TeamSymbols::execute()
{
}

double TeamSymbols::getTeamMembersAliveCount()
{
  int counter = 0;

  for(auto const &it : theInstance->getTeamMessage().data) {
    const TeamMessageData& messageData = it.second;

    // "alive" means sent something in the last n seconds
    if(theInstance->getFrameInfo().getTimeSince(messageData.frameInfo.getTime()) < theInstance->parameters.maximumFreshTime)
    {
      counter++;
    }
  }//end for

  return (double) counter;
}//end getTeamMembersAliveCount

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

TeamSymbols::~TeamSymbols()
{
}
