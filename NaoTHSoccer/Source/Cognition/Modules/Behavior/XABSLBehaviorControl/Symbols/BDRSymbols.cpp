/**
 * @file BDRSymbols.cpp
 *
 * @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
 * Implementation of class BDRSymbols
 */

#include "BDRSymbols.h"
#include <string>
#include <sstream>

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
  engine.registerDecimalInputSymbol("bdr.number_of_players_in_servicing", &getNumberOfPlayersInServicing);

  engine.registerDecimalOutputSymbol("bdr.time_playing", &getBDRPlayerState().time_playing);
  engine.registerDecimalInputSymbol("bdr.goalsOwn", &getGoalsOwn);
  engine.registerDecimalInputSymbol("bdr.goalsOpp", &getGoalsOpp);

  for(int i = 0; i < BDRPlayerState::num_of_reasons; i++)
  {
    std::string str("reason.");
    str.append(BDRPlayerState::getReasonName((BDRPlayerState::Reason)i));
    engine.registerEnumElement("reason", str.c_str(), i);
  }

  engine.registerEnumeratedOutputSymbol("bdr.reason", "reason", &setBDRReason, &getBDRReason);

  engine.registerDecimalOutputSymbol("bdr.wartung_max_target", &getBDRPlayerState().wartung_max_target);
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
    //theInstance->getSayRequest().setMessage(theInstance->getBDRPlayerState().getMessageString(m), theInstance->getFrameInfo());
    std::ostringstream ss;
    ss << "bdr" << value << ".wav";
    theInstance->getSoundPlayData().soundFile = ss.str();
  }
  theInstance->getBDRPlayerState().message = m;
}

int BDRSymbols::getBDRMessage() {
  return static_cast<int>(theInstance->getBDRPlayerState().message);
}

void BDRSymbols::setBDRReason(int value) {
  assert(value >= 0 && value < static_cast<int>(BDRPlayerState::num_of_reasons));
  theInstance->getBDRPlayerState().wartungs_reason = static_cast<BDRPlayerState::Reason>(value);
}

int BDRSymbols::getBDRReason() {
  return static_cast<int>(theInstance->getBDRPlayerState().wartungs_reason);
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

double BDRSymbols::getNumberOfPlayersInServicing() {

  int count = 0;
  for (auto const& i : theInstance->getTeamMessage().data) {
    if(i.second.custom.bdrPlayerState.activity == BDRPlayerState::servicing) {
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
