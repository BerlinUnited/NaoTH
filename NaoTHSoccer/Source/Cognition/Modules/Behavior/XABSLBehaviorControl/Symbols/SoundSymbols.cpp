/**
 * @file SoundSymbols.cpp
 *
 * Implementation of class SoundSymbols
  *
 * Currently not used
 * Experimental feature
 *
 */


#include "SoundSymbols.h"

void SoundSymbols::registerSymbols(xabsl::Engine& engine)
{
  // register the sound enums
  engine.registerEnumElement("sound", "sound.off", 0);
  engine.registerEnumElement("sound", "sound.victory", 1);
  engine.registerEnumElement("sound", "sound.asta_la_vista", 2);
  engine.registerEnumElement("sound", "sound.penalized", 3);
  engine.registerEnumElement("sound", "sound.playing", 4);
  engine.registerEnumElement("sound", "sound.battery_low", 5);

  engine.registerEnumeratedOutputSymbol("sound.request", "sound", &setSoundRequest, &getSoundRequest);
}//end registerSymbols

SoundSymbols* SoundSymbols::theInstance = NULL;

void SoundSymbols::execute()
{
}

void SoundSymbols::setSoundRequest(int value)
{
  if(value == 0)
  {
    theInstance->getSoundPlayData().mute = true;
    theInstance->getSoundPlayData().soundFile = "";
  }
  else
  {
    theInstance->getSoundPlayData().mute = false;
    if(value == 1) {
      theInstance->getSoundPlayData().soundFile = "victory.wav";
    } else if(value == 2) {
      theInstance->getSoundPlayData().soundFile = "asta_la_vista.wav";
    } else if(value == 3) {
      theInstance->getSoundPlayData().soundFile = "penalized.wav";
    } else if(value == 4) {
      theInstance->getSoundPlayData().soundFile = "play.wav";
    } else if(value == 5) {
      theInstance->getSoundPlayData().soundFile = "battery_low.wav";
    }
  }
}

int SoundSymbols::getSoundRequest()
{
  if(theInstance->getSoundPlayData().soundFile == "") {
    return 0;
  } else if(theInstance->getSoundPlayData().soundFile == "victory.wav") {
    return 1;
  } else if(theInstance->getSoundPlayData().soundFile == "asta_la_vista.wav") {
    return 2;
  } else if(theInstance->getSoundPlayData().soundFile == "penalized.wav") {
    return 3;
  } else if(theInstance->getSoundPlayData().soundFile == "play.wav") {
    return 4;
  } else if(theInstance->getSoundPlayData().soundFile == "battery_low.wav") {
    return 5;
  }

  return 0;
}


