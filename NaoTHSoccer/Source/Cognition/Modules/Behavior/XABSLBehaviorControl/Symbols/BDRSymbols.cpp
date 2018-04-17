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
  // enum type for motion
  for(int i = 0; i < BDRPlayerState::num_of_activities; i++)
  {
    std::string str("activity.");
    str.append(BDRPlayerState::getActivityName((BDRPlayerState::Activity)i));
    engine.registerEnumElement("activity", str.c_str(), i);
  }

  engine.registerEnumeratedOutputSymbol("bdr.activity", "activity", &setBDRActivity, &getBDRActivity);

  engine.registerBooleanOutputSymbol("bdr.sitting",            &getBDRPlayerState().sitting);
  engine.registerBooleanOutputSymbol("bdr.localized_on_field", &getBDRPlayerState().localized_on_field);

}//end registerSymbols

BDRSymbols* BDRSymbols::theInstance = NULL;

void BDRSymbols::setBDRActivity(int value) {
  assert(value >= 0 && value < static_cast<int>(BDRPlayerState::num_of_activities));
  theInstance->getBDRPlayerState().activity = static_cast<BDRPlayerState::Activity>(value);
}

int BDRSymbols::getBDRActivity() {
  return static_cast<int>(theInstance->getBDRPlayerState().activity);
}
