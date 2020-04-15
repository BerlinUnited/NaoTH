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
    
    TeamMessageData messageData;
    messageData.playerNumber = robotNumber;
    messageData.frameInfo = getFrameInfo();

    getTeamMessage().data[messageData.playerNumber] = messageData;

    //Generate the frame number, at which we are going to receive the next message
    if (params.normalDistribution) {
      getFrameInfo().getFrameNumber();
      nextMessageTime = getFrameInfo().getTime() + ((unsigned int) fabs(floor(
        Math::sampleNormalDistribution(params.standardDeviation) + params.mean + 0.5)));
    }
    if (params.uniformDistribution) {      
      nextMessageTime = getFrameInfo().getTime() + ((unsigned int) fabs(floor(
        Math::random(params.uniformMin, params.uniformMax) + 0.5)));
    }

    if (params.randomPerturbations) {
      //TODO: Implement other perturbations in the message intervals
    }

  }//endIf

}//endExecute

void TeamCommReceiveEmulator::reset() {

}
