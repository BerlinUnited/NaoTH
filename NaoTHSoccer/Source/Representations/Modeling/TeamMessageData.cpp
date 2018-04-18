#include "TeamMessageData.h"
#include <Tools/DataConversion.h>

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
    ballAge(-1)
{}

SPLStandardMessage TeamMessageData::createSplMessage() const
{
    SPLStandardMessage spl;

    spl.playerNum = (uint8_t) playerNumber;
    spl.teamNum = (uint8_t) teamNumber;

    spl.pose[0] = (float) pose.translation.x;
    spl.pose[1] = (float) pose.translation.y;
    spl.pose[2] = (float) pose.rotation;

    // in seconds (only if positive)!
    spl.ballAge = (float) ((ballAge < 0)? ballAge : ballAge / 1000.0);
    spl.ball[0] = (float) ballPosition.x;
    spl.ball[1] = (float) ballPosition.y;

    spl.fallen = (uint8_t) fallen;

    // user defined data, this includes our own data and the DoBerMan mixed team common header
    naothmessages::BUUserTeamMessage userMsgBU = custom.toProto();
    std::string userMsgDoBer = custom.toDoBerManHeader();
    size_t buUserSize = userMsgBU.ByteSize();
    size_t doberUserSize = userMsgDoBer.size();
    size_t userSize = buUserSize + doberUserSize;
    if (userSize < SPL_STANDARD_MESSAGE_DATA_SIZE) {
        spl.numOfDataBytes = static_cast<uint16_t>(userSize);

        // 1. write custom data as DoBerMan header
        memcpy(spl.data, userMsgDoBer.c_str(), doberUserSize);

        // 2. write custom data in BerlinUnited format
        userMsgBU.SerializeToArray(spl.data + doberUserSize, static_cast<int>(userSize));
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
    fallen = (spl.fallen == 1);

    // parses the user-defined part of a SplMessage
    ASSERT(spl.numOfDataBytes <= SPL_STANDARD_MESSAGE_DATA_SIZE);

    const size_t customOffset = TeamMessageCustom::getCustomOffset();

    // parse DoBerMan part of user data
    custom.parseFromDoBerManHeader(spl.data, spl.numOfDataBytes);

    // parse BerlinUnited part of user data
    if(spl.numOfDataBytes > customOffset)
    {
      naothmessages::BUUserTeamMessage userData;
      try
      {
        if (userData.ParseFromArray(spl.data + customOffset, static_cast<int>(spl.numOfDataBytes)-static_cast<int>(customOffset))) {
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
    }

    return true; // return "success"
}

void TeamMessageData::print(std::ostream &stream) const
{
    stream << "TeamMessageData from " << playerNumber << "\n";
    //frameInfo.print(stream);
    stream << "\t" << "last received = "
                   << frameInfo.getFrameNumber()
                   << " @ " << frameInfo.getTime() << "\n"
                   << "Parsed at: " << timestampParsed << "\n"
           << "\t" << "Pos (x; y; rotation) = "
                   << pose.translation.x << "; "
                   << pose.translation.y << "; "
                   << pose.rotation <<"\n"
           << "\t" << "Ball (x; y) = "
                   << ballPosition.x << "; "
                   << ballPosition.y <<"\n"
           << "\t" << "TimeSinceBallwasSeen: " << ballAge <<"\n"
           << "\t" << "fallenDown: " << (fallen ? "yes" : "no") <<"\n"
           << "\t" << "team number: " << teamNumber <<"\n"
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
  temperature(0.0),
  cpuTemperature(0.0),
  whistleDetected(false),
  whistleCount(0),
  // init with "invalid" position
  teamBall(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()),
  isCharging(false),
  robotState(PlayerInfo::initial),
  teamColor(naoth::GameData::unknown_team_color)
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
    << "\t" << "temperature: " << temperature << "°C\n"
    << "\t" << "CPU: " << cpuTemperature << "°C\n"
    << "\t" << "whistleDetected: " << (whistleDetected ? "yes" : "no") << "\n"
    << "\t" << "whistleCount: " << whistleCount << "\n"
    << "\t" << "teamball position: "
        << teamBall.x << "/" << teamBall.y << "\n";
  if(!ntpRequests.empty()) {
      stream << "\t" << "ntp request for: \n";
      for(auto const& request : ntpRequests) {
          stream << "\t\t" << request.playerNumber << ", " << request.sent << " -> " << request.received << std::endl;
      }
  }

  stream << "RobotState" << PlayerInfo::toString(robotState) << "\n";
  
  bdrPlayerState.print(stream);

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
    userMsg.set_cputemperature((float)cpuTemperature);
    userMsg.set_whistledetected(whistleDetected);
    userMsg.set_whistlecount(whistleCount);
    DataConversion::toMessage(teamBall, *(userMsg.mutable_teamball()));
    // only if any sync data is set
    if(ntpRequests.size() > 0)
    {
        // ... set the time syncing info
        for(auto const& request: ntpRequests) {
            auto msgPlayer = userMsg.add_ntprequest();
            msgPlayer->set_playernum(request.playerNumber);
            msgPlayer->set_sent(request.sent);
            msgPlayer->set_received(request.received);
          }
    }

    userMsg.set_ischarging(isCharging);
    userMsg.set_message(message);

    userMsg.set_key(key);
    userMsg.set_robotstate((naothmessages::RobotState)robotState);
    userMsg.set_teamcolor((naothmessages::TeamColor)teamColor);

    userMsg.mutable_bdrplayerstate()->set_activity((naothmessages::BDRPlayerState_Activity)bdrPlayerState.activity);
    userMsg.mutable_bdrplayerstate()->set_sitting(bdrPlayerState.sitting);
    userMsg.mutable_bdrplayerstate()->set_localized_on_field(bdrPlayerState.localized_on_field);

    return userMsg;
}

void TeamMessageCustom::parseFromDoBerManHeader(const uint8_t* rawHeader, size_t headerSize)
{
  // initialize with some values so the compiler is happy
  DoBerManHeader header = {timestamp, 0, isPenalized, whistleDetected, 0};
  if(headerSize >= sizeof(DoBerManHeader))
  {
    memcpy(&header, rawHeader, sizeof(DoBerManHeader));
  }
  // copy the parsed data
  timestamp = header.timestamp;
  isPenalized = (header.isPenalized > 0);
  whistleDetected = (header.whistleDetected > 0);
}

std::string TeamMessageCustom::toDoBerManHeader() const
{
  // copy the information into an internal struct
  DoBerManHeader header;
  header.timestamp = timestamp;
  // TODO: make the DoBerMan team ID configurable, now it is fixed to 4
  header.teamID = 4;
  header.isPenalized = isPenalized;
  header.whistleDetected = whistleDetected;

  // create the result byte array by mapping the header struct
  std::string result;
  result.assign((char*)&header, sizeof(DoBerManHeader));

  // we don't need any of this data anymore, move it
  return std::move(result);
}

void TeamMessageCustom::parseFromProto(const naothmessages::BUUserTeamMessage &userData)
{
    // basic info
    key = userData.key();
    timestamp = userData.timestamp();
    bodyID = userData.bodyid();
    
    // body status
    batteryCharge = userData.batterycharge();
    temperature = userData.temperature();
    cpuTemperature = userData.cputemperature();

    // strategic info
    timeToBall = userData.timetoball();
    wasStriker = userData.wasstriker();
    wantsToBeStriker = userData.wantstobestriker();
    
    isPenalized = userData.ispenalized(); // TODO: remove and use game data instead
    
    whistleDetected = userData.whistledetected();
    whistleCount = userData.whistlecount();
    if(userData.has_teamball()) {
        DataConversion::fromMessage(userData.teamball(),teamBall);
    } else {
        teamBall.x = std::numeric_limits<double>::infinity();
        teamBall.y = std::numeric_limits<double>::infinity();
    }

    ntpRequests = std::vector<NtpRequest>(userData.ntprequest_size());
    if(userData.ntprequest_size() > 0) {
        for(int i=0; i < userData.ntprequest_size(); i++) {
            auto& request = userData.ntprequest((int)i);
            auto& syncingPlayer = ntpRequests[i];
            syncingPlayer.playerNumber = request.playernum();
            syncingPlayer.sent = request.sent();
            syncingPlayer.received = request.received();
        }
    }

    isCharging = userData.ischarging();
    message = userData.message();

    robotState = (PlayerInfo::RobotState) userData.robotstate();
	teamColor = (naoth::GameData::TeamColor) userData.teamcolor();
	
    bdrPlayerState.activity = (BDRPlayerState::Activity) userData.bdrplayerstate().activity();
    bdrPlayerState.sitting = userData.bdrplayerstate().sitting();
    bdrPlayerState.localized_on_field = userData.bdrplayerstate().localized_on_field();
}

