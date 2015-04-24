/**
* @file TeamCommReceiveEmulator.cpp
*/

#include "TeamCommReceiveEmulator.h"

unsigned int robotNumber = 999; //An arbitrary robot number that does not stand in conflict with real robots' messages

TeamCommReceiveEmulator::TeamCommReceiveEmulator():
nextMessageTime(getFrameInfo().getTime())
{}

TeamCommReceiveEmulator::~TeamCommReceiveEmulator()
{}

void TeamCommReceiveEmulator::execute() {

  //If enough time has passed, place another message in the inbox
  if (nextMessageTime <= getFrameInfo().getTime()) {
    
    TeamMessage::Data messageData;
    messageData.playerNum = robotNumber;
    messageData.frameInfo = getFrameInfo();

    getTeamMessage().data[messageData.playerNum] = messageData;

    //Generate the frame number, at which we are going to receive the next message
    if (parameters.normalDistribution) {
      getFrameInfo().getFrameNumber();
      nextMessageTime = getFrameInfo().getTime() + ((unsigned int) fabs(floor(
        Math::sampleNormalDistribution(parameters.standardDeviation) + parameters.mean + 0.5)));
    }
    if (parameters.uniformDistribution) {      
      nextMessageTime = getFrameInfo().getTime() + ((unsigned int) fabs(floor(
        Math::random(parameters.uniformMin, parameters.uniformMax) + 0.5)));
    }

    if (parameters.randomPerturbations) {
      //TODO: Implement other perturbations in the message intervals
    }

  }//endIf

}//endExecute

void TeamCommReceiveEmulator::reset() {

}