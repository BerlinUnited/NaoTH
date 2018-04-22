/**
 * @file BDRSymbols.cpp
 *
 * @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
 * Implementation of class BDRSymbols
 */

#include "BDRSymbols.h"
#include <string>

void BDRSymbols::registerSymbols(xabsl::Engine& engine)
{
  // enum type for bdr activities
  for(int i = 0; i < BDRPlayerState::num_of_activities; i++)
  {
    std::string str("activity.");
    str.append(BDRPlayerState::getActivityName((BDRPlayerState::Activity)i));
    engine.registerEnumElement("activity", str.c_str(), i);
  }

  engine.registerEnumeratedOutputSymbol("bdr.activity", "activity", &setBDRActivity, &getBDRActivity);

  for(int i = 0; i < BDRPlayerState::num_of_messages; i++)
  {
    std::string str("message.");
    str.append(BDRPlayerState::getMessageName((BDRPlayerState::Message)i));
    engine.registerEnumElement("message", str.c_str(), i);
  }

  engine.registerEnumeratedOutputSymbol("bdr.message", "message", &setBDRMessage, &getBDRMessage);

  engine.registerBooleanOutputSymbol("bdr.sitting",            &getBDRPlayerState().sitting);
  engine.registerBooleanOutputSymbol("bdr.localized_on_field", &getBDRPlayerState().localized_on_field);
  engine.registerDecimalInputSymbol("bdr.number_of_players_in_play", &getNumberOfPlayersInPlay);
  
  engine.registerDecimalOutputSymbol("bdr.time_playing", &getBDRPlayerState().time_playing);
  engine.registerDecimalInputSymbol("bdr.goalsOwn", &getGoalsOwn);
  engine.registerDecimalInputSymbol("bdr.goalsOpp", &getGoalsOpp);
}//end registerSymbols

BDRSymbols* BDRSymbols::theInstance = NULL;

void BDRSymbols::setBDRActivity(int value) {
  assert(value >= 0 && value < static_cast<int>(BDRPlayerState::num_of_activities));
  theInstance->getBDRPlayerState().activity = static_cast<BDRPlayerState::Activity>(value);
}

int BDRSymbols::getBDRActivity() {
  return static_cast<int>(theInstance->getBDRPlayerState().activity);
}

void BDRSymbols::setBDRMessage(int value) {
  assert(value >= 0 && value < static_cast<int>(BDRPlayerState::num_of_messages));
  BDRPlayerState::Message m = static_cast<BDRPlayerState::Message>(value);
  if(theInstance->getBDRPlayerState().message != m){
    theInstance->getSayRequest().setMessage(theInstance->getBDRPlayerState().getMessageString(m), theInstance->getFrameInfo());
  }
  theInstance->getBDRPlayerState().message = m;
}

int BDRSymbols::getBDRMessage() {
  return static_cast<int>(theInstance->getBDRPlayerState().message);
}

double BDRSymbols::getNumberOfPlayersInPlay() {
  
  int count = 0;
  for (auto const& i : theInstance->getTeamMessage().data) {
    if(i.second.custom.bdrPlayerState.activity == BDRPlayerState::playing) {
      ++count;
    }
  }
  
  return count;
}

double BDRSymbols::getGoalsOwn() {
  
  auto const& i = theInstance->getTeamMessage().data.find(0);
  if(i != theInstance->getTeamMessage().data.end() && theInstance->getRobotPose().isValid) {
    if (theInstance->getRobotPose().globallyMirrored) {
      return i->second.custom.bdrPlayerState.goalsRight;
    } else {
      return i->second.custom.bdrPlayerState.goalsLeft; 
    }
  }
  return 0;
}

double BDRSymbols::getGoalsOpp() {
  auto const& i = theInstance->getTeamMessage().data.find(0);
  if(i != theInstance->getTeamMessage().data.end() && theInstance->getRobotPose().isValid) {
    if (theInstance->getRobotPose().globallyMirrored) {
      return i->second.custom.bdrPlayerState.goalsLeft;
    } else {
      return i->second.custom.bdrPlayerState.goalsRight; 
    }
  }
  return 0;
}
