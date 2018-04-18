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
  theInstance->getBDRPlayerState().message = static_cast<BDRPlayerState::Message>(value);
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
