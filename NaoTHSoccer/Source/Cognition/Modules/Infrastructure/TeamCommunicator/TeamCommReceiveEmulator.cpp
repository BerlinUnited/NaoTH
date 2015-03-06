/**
* @file TeamCommReceiveEmulator.cpp
*/

#include "TeamCommReceiveEmulator.h"

unsigned int robotNumber = 999;

TeamCommReceiveEmulator::TeamCommReceiveEmulator():
nextMessage(getFrameInfo().getTime())
{}

TeamCommReceiveEmulator::~TeamCommReceiveEmulator()
{}

void TeamCommReceiveEmulator::execute() {

  //If enough time has passed, place another message in the inbox
  if (nextMessage <= getFrameInfo().getTime()) {
    
    TeamMessage::Data messageData;
    messageData.playerNum = robotNumber;
    messageData.frameInfo = getFrameInfo();

    getTeamMessage().data[messageData.playerNum] = messageData;

    //Generate the frame number, at which we are going to receive the next message
    if (parameters.normalDistribution) {
      getFrameInfo().getFrameNumber();
      nextMessage = getFrameInfo().getTime() + ((unsigned int) abs(floor(
        Math::sampleNormalDistribution(parameters.standardDeviation) + parameters.mean + 0.5)));
    }
    if (parameters.uniformDistribution) {      
      nextMessage = getFrameInfo().getTime() + ((unsigned int) abs(floor(
        Math::random(parameters.uniformMin, parameters.uniformMax) + 0.5)));
    }

  }//endIf

}//endExecute

void TeamCommReceiveEmulator::reset() {

}