/**
 * @file SoundSymbols.cpp
 *
 * Implementation of class SoundSymbols
 */


#include "SoundSymbols2011.h"
#include "LedSymbols2011.h"

void SoundSymbols2011::registerSymbols(xabsl::Engine& engine)
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



SoundSymbols2011* SoundSymbols2011::theInstance = NULL;

void SoundSymbols2011::execute()
{
}


void SoundSymbols2011::setSoundRequest(int value)
{
  if(value == 0)
  {
    theInstance->soundData.mute = true;
    theInstance->soundData.soundFile = "";
  }
  else
  {
    theInstance->soundData.mute = false;
    if(value == 1)
    {
      theInstance->soundData.soundFile = "victory.wav";
    }
    else if(value == 2)
    {
      theInstance->soundData.soundFile = "asta_la_vista.wav";
    }
    else if(value == 3)
    {
      theInstance->soundData.soundFile = "penalized.wav";
    }
    else if(value == 4)
    {
      theInstance->soundData.soundFile = "play.wav";
    }
    else if(value == 5)
    {
      theInstance->soundData.soundFile = "battery_low.wav";
    }
  }
}

int SoundSymbols2011::getSoundRequest()
{
  if(theInstance->soundData.soundFile == "")
  {
    return 0;
  }
  else if(theInstance->soundData.soundFile == "victory.wav")
  {
    return 1;
  }
  else if(theInstance->soundData.soundFile == "asta_la_vista.wav")
  {
    return 2;
  }
  else if(theInstance->soundData.soundFile == "penalized.wav")
  {
    return 3;
  }
  else if(theInstance->soundData.soundFile == "play.wav")
  {
    return 4;
  }
  else if(theInstance->soundData.soundFile == "battery_low.wav")
  {
    return 5;
  }

  return 0;
}


