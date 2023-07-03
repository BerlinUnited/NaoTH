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

#include <string>

void SoundSymbols::registerSymbols(xabsl::Engine& engine)
{
  // register the sound enums
  engine.registerEnumElement("sound", "sound.off", 0);
  engine.registerEnumElement("sound", "sound.victory", 1);
  engine.registerEnumElement("sound", "sound.asta_la_vista", 2);
  engine.registerEnumElement("sound", "sound.penalized", 3);
  engine.registerEnumElement("sound", "sound.playing", 4);
  engine.registerEnumElement("sound", "sound.battery_low", 5);
  engine.registerEnumElement("sound", "sound.weeeee", 6);
  engine.registerEnumElement("sound", "sound.finished_walking", 7);
  engine.registerEnumElement("sound", "sound.ball_position", 8);
  engine.registerEnumElement("sound", "sound.start_calibration", 9);

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
    } else if(value == 6) {
      theInstance->getSoundPlayData().soundFile = "weeeee.wav";
    } else if(value == 7) {
      theInstance->getSoundPlayData().soundFile = ":Finished walking";
    } else if(value == 8) {

      theInstance->getSoundPlayData().soundFile = ":The ball is at: ";

      Vector2d ball(0,0);
      if(theInstance->getBallModel().knows) {
        ball = theInstance->getRobotPose()*theInstance->getBallModel().position;
        ball = ball / 10.0;
      }

      theInstance->getSoundPlayData().soundFile += 
        std::to_string(static_cast<int>(ball.x)) + " " + 
        std::to_string(static_cast<int>(ball.y));
    } else if(value == 9) {
      theInstance->getSoundPlayData().soundFile = ":Start Calibration";
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
  } else if(theInstance->getSoundPlayData().soundFile == "weeeee.wav") {
    return 6;
  } else if(theInstance->getSoundPlayData().soundFile == "finished_walking") {
    return 7;
  } else if(theInstance->getSoundPlayData().soundFile == "ball_position") {
    return 8;
  } else if(theInstance->getSoundPlayData().soundFile == "start_calibration") {
    return 9;
  }

  return 0;
}


