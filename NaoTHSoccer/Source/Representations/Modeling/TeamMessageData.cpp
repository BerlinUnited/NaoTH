#include "TeamMessageData.h"

#include <limits>

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
    sideConfidence(0),
    
    /* BU-Fields *************************************************************/
    timestamp(0),
    bodyID("unknown"),
    wantsToBeStriker(false),
    timeToBall(std::numeric_limits<unsigned int>::max()),
    isPenalized(false),
    batteryCharge(0.0),
    temperature(0.0)
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

    // user defined data
    naothmessages::BUUserTeamMessage userMsg = getBUUserTeamMessage();
    int userSize = userMsg.ByteSize();
    if(spl.numOfDataBytes < SPL_STANDARD_MESSAGE_DATA_SIZE)
    {
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

bool TeamMessageData::parseFromSplMessage(const SPLStandardMessage &spl, int team, int player)
{
    playerNumber = spl.playerNum;
    teamNumber = spl.teamNum;

    if( (team > 0 && teamNumber != team) ||        // only parse messages from this "team"
        (team < 0 && teamNumber == abs(team)) ||   // ignore messages from this "team"
        (player > 0 && playerNumber != player) ||  // only parse messages from this "player"
        (player < 0 && playerNumber == abs(player))// ignore messages from this "player"
    ) {
        return false;
    }

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
    parseFromSplMessageData(spl.data, spl.numOfDataBytes);

    // return "success"
    return true;
}

bool TeamMessageData::parseFromSplMessageString(const std::string &data, int team, int player)
{
    SPLStandardMessage spl;

    // invalid message size
    if(data.size() > sizeof(SPLStandardMessage)) {
        std::cout << "invalid size" << std::endl;
      return false;
    }

    // convert back to struct
    memcpy(&spl, data.c_str(), data.size());

    // furter sanity check for header and version
    if(spl.header[0] != 'S' ||
       spl.header[1] != 'P' ||
       spl.header[2] != 'L' ||
       spl.header[3] != ' ' ||
       spl.version != SPL_STANDARD_MESSAGE_STRUCT_VERSION)
    {
        std::cout << "invalid header/version" << std::endl;
      return false;
    }
    return parseFromSplMessage(spl, team, player);
}

void TeamMessageData::print(std::ostream &stream) const
{
    stream << "TeamMessageData from " << playerNumber << " ["<<bodyID<<"]"<<"\n";
    frameInfo.print(stream);
    stream << "Message: "<<"\n"
           << "\t" << "Timestamp: " << timestamp <<"\n"
           << "\t" << "Pos (x; y; rotation) = "
                   << pose.translation.x << "; "
                   << pose.translation.y << "; "
                   << pose.rotation <<"\n"
           << "\t" << "Ball (x; y) = "
                   << ballPosition.x << "; "
                   << ballPosition.y <<"\n"
           << "\t" << "TimeSinceBallwasSeen: " << ballAge <<"\n"
           << "\t" << "TimeToBall: "<< timeToBall <<"\n"
           << "\t" << "wasStriker: " << (wantsToBeStriker ? "yes" : "no") <<"\n"
           << "\t" << "isPenalized: " << (isPenalized ? "yes" : "no") <<"\n"
           << "\t" << "fallenDown: " << (fallen ? "yes" : "no") <<"\n"
           << "\t" << "team number: " << teamNumber <<"\n"
           << "\t" << "expectedBallPos (x; y) = "
                   << shootingTo.x << "; "
                   << shootingTo.y <<"\n"
           // TODO: add some more infos ???
          ;
    stream << std::endl;
}//end print

naothmessages::BUUserTeamMessage TeamMessageData::getBUUserTeamMessage() const
{
    naothmessages::BUUserTeamMessage userMsg;
    userMsg.set_bodyid(bodyID);
    userMsg.set_wasstriker(wantsToBeStriker);
    userMsg.set_timestamp(timestamp);
    userMsg.set_timetoball((unsigned int)timeToBall);
    userMsg.set_ispenalized(isPenalized);
    userMsg.set_batterycharge((float)batteryCharge);
    userMsg.set_temperature((float)temperature);
    return userMsg;
}

void TeamMessageData::parseFromBUUserTeamMessage(const naothmessages::BUUserTeamMessage &userData)
{
    timestamp = userData.timestamp();
    bodyID = userData.bodyid();
    wantsToBeStriker = userData.wasstriker();
    timeToBall = userData.timetoball();
    isPenalized = userData.ispenalized();
    batteryCharge = userData.batterycharge();
    temperature = userData.temperature();
}

void TeamMessageData::parseFromSplMessageData(const unsigned char* data, uint16_t size)
{
    // check if we can deserialize the user defined data
    if(size > 0 && size <= SPL_STANDARD_MESSAGE_DATA_SIZE)
    {
        naothmessages::BUUserTeamMessage userData;
        try
        {
            if(userData.ParseFromArray(data, size))
            {
                parseFromBUUserTeamMessage(userData);
            }
        } catch(...) {
            // well, this is not one of our messages, ignore

            // TODO: we might want to maintain a list of robots which send
            // non-compliant messages in order to avoid overhead when trying to parse it
        }
    }
}
