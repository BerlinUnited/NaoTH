#include "TeamMessageData.h"

#include <limits>

using namespace naoth;


TeamMessageData::TeamMessageData():TeamMessageData(FrameInfo())
{}

TeamMessageData::TeamMessageData(FrameInfo fi):
    frameInfo(fi),
    /* SPL-Message-Fields ****************************************************/
    playerNumber(0),
    teamNumber(0),
    fallen(false),
    ballAge(-1),
    suggestion(SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS, 0),
    intention(0),
    averageWalkSpeed(160),
    maxKickDistance(3000),
    positionConfidence(0),
    sideConfidence(0)
{}

SPLStandardMessage TeamMessageData::createSplMessage() const
{
    SPLStandardMessage spl;

    spl.playerNum = (uint8_t) playerNumber;
    spl.teamNum = (uint8_t) teamNumber;

    spl.pose[0] = (float) pose.translation.x;
    spl.pose[1] = (float) pose.translation.y;
    spl.pose[2] = (float) pose.rotation;

    spl.currentPositionConfidence = (uint8_t) positionConfidence;
    spl.currentSideConfidence = (uint8_t) sideConfidence;

    // in seconds!
    spl.ballAge = (float) (ballAge / 1000.0);
    spl.ball[0] = (float) ballPosition.x;
    spl.ball[1] = (float) ballPosition.y;
    spl.ballVel[0] = (float) ballVelocity.x;
    spl.ballVel[1] = (float) ballVelocity.y;

    spl.fallen = (uint8_t) fallen;

    spl.walkingTo[0] = (float) walkingTo.x;
    spl.walkingTo[1] = (float) walkingTo.y;

    spl.shootingTo[0] = (float) shootingTo.x;
    spl.shootingTo[1] = (float) shootingTo.y;

    ASSERT(suggestion.size() == SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS);
    for(int i = 0; i < SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS; ++i) {
        spl.suggestion[i] = (int8_t) suggestion[i];
    }

    spl.intention = (uint8_t) intention;

    spl.averageWalkSpeed = (int16_t) averageWalkSpeed;
    spl.maxKickDistance = (int16_t) maxKickDistance;

    // TODO: 
    // user defined data
    naothmessages::BUUserTeamMessage userMsg = custom.toProto();
    int userSize = userMsg.ByteSize();
    if (userSize < SPL_STANDARD_MESSAGE_DATA_SIZE) {
        spl.numOfDataBytes = (uint16_t) userSize;
        userMsg.SerializeToArray(spl.data, userSize);
    } else {
        spl.numOfDataBytes = 0;
    }

    ASSERT(spl.numOfDataBytes <= SPL_STANDARD_MESSAGE_DATA_SIZE);

    return spl;
}

std::string TeamMessageData::createSplMessageString() const
{
    SPLStandardMessage spl = createSplMessage();
    std::string msg;
    // copy only the part of the message which is actually used
    msg.assign((char*)&spl, sizeof(SPLStandardMessage) - SPL_STANDARD_MESSAGE_DATA_SIZE + spl.numOfDataBytes);
    return msg;
}

bool TeamMessageData::parseFromSplMessage(const SPLStandardMessage &spl)
{
    playerNumber = spl.playerNum;
    teamNumber = spl.teamNum;

    // parses the standard fields of a SplMessage
    pose.translation.x = spl.pose[0];
    pose.translation.y = spl.pose[1];
    pose.rotation = spl.pose[2];

    ballAge = spl.ballAge * 1000.0f;
    ballPosition.x = spl.ball[0];
    ballPosition.y = spl.ball[1];
    ballVelocity.x = spl.ballVel[0];
    ballVelocity.y = spl.ballVel[1];

    fallen = (spl.fallen == 1);

    walkingTo.x = spl.walkingTo[0];
    walkingTo.y = spl.walkingTo[1];

    shootingTo.x = spl.shootingTo[0];
    shootingTo.y = spl.shootingTo[1];

    suggestion.assign(spl.suggestion, spl.suggestion+SPL_STANDARD_MESSAGE_MAX_NUM_OF_PLAYERS);
    intention = spl.intention;

    averageWalkSpeed = spl.averageWalkSpeed;
    maxKickDistance = spl.maxKickDistance;

    positionConfidence = spl.currentPositionConfidence;
    sideConfidence = spl.currentSideConfidence;

    // parses the user-defined part of a SplMessage
    ASSERT(spl.numOfDataBytes <= SPL_STANDARD_MESSAGE_DATA_SIZE);
    naothmessages::BUUserTeamMessage userData;
    
    try
    {
      if (userData.ParseFromArray(spl.data, spl.numOfDataBytes)) {
          custom.parseFromProto(userData);
      } else {
        return false;
      }
    } 
    catch (...) {
      // well, this is not one of our messages, ignore

      // TODO: we might want to maintain a list of robots which send
      // non-compliant messages in order to avoid overhead when trying to parse it
      //std::cout << "could not parse custom part" << std::endl;
      return false;
    }

    return true; // return "success"
}

void TeamMessageData::print(std::ostream &stream) const
{
    stream << "TeamMessageData from " << playerNumber << "\n";
    //frameInfo.print(stream);
    stream << "\t" << "Pos (x; y; rotation) = "
                   << pose.translation.x << "; "
                   << pose.translation.y << "; "
                   << pose.rotation <<"\n"
           << "\t" << "Ball (x; y) = "
                   << ballPosition.x << "; "
                   << ballPosition.y <<"\n"
           << "\t" << "TimeSinceBallwasSeen: " << ballAge <<"\n"
           << "\t" << "fallenDown: " << (fallen ? "yes" : "no") <<"\n"
           << "\t" << "team number: " << teamNumber <<"\n"
           << "\t" << "expectedBallPos (x; y) = "
                   << shootingTo.x << "; "
                   << shootingTo.y <<"\n"
          ;
    
    custom.print(stream);
}//end print


TeamMessageCustom::TeamMessageCustom() :
  timestamp(0),
  bodyID("unknown"),
  wasStriker(false),
  wantsToBeStriker(false),
  timeToBall(std::numeric_limits<unsigned int>::max()),
  isPenalized(false),
  batteryCharge(0.0),
  temperature(0.0)
{
}

void TeamMessageCustom::print(std::ostream &stream) const
{
  stream << "TeamMessageCustom \n";
  stream
    << "\t" << "bodyID: " << bodyID << "\n"
    << "\t" << "Timestamp: " << timestamp << "\n"
    << "\t" << "TimeToBall: " << timeToBall << "\n"
    << "\t" << "wasStriker: " << (wasStriker ? "yes" : "no") << "\n"
    << "\t" << "wantsToBeStriker: " << (wantsToBeStriker ? "yes" : "no") << "\n"
    << "\t" << "isPenalized: " << (isPenalized ? "yes" : "no") << "\n"
    << "\t" << "batteryCharge: " << batteryCharge << "\n"
    << "\t" << "temperature: " << temperature << "\n"
    ;
  stream << std::endl;
}//end print


naothmessages::BUUserTeamMessage TeamMessageCustom::toProto() const
{
    naothmessages::BUUserTeamMessage userMsg;
    userMsg.set_bodyid(bodyID);
    userMsg.set_wasstriker(wasStriker);
    userMsg.set_wantstobestriker(wantsToBeStriker);
    userMsg.set_timestamp(timestamp);
    userMsg.set_timetoball(timeToBall);
    userMsg.set_ispenalized(isPenalized);
    userMsg.set_batterycharge((float)batteryCharge);
    userMsg.set_temperature((float)temperature);
    return userMsg;
}

void TeamMessageCustom::parseFromProto(const naothmessages::BUUserTeamMessage &userData)
{
    timestamp = userData.timestamp();
    bodyID = userData.bodyid();
    wasStriker = userData.wasstriker();
    wantsToBeStriker = userData.wantstobestriker();
    timeToBall = userData.timetoball();
    isPenalized = userData.ispenalized();
    batteryCharge = userData.batterycharge();
    temperature = userData.temperature();
    key = userData.key();
}

