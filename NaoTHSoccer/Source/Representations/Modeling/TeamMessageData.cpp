#include "TeamMessageData.h"
#include <Tools/DataConversion.h>

#include <limits>
#include <iomanip>

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

    // user defined data, this includes our own data and the mixed team common header
    naothmessages::BUUserTeamMessage userMsgBU = custom.toProto();
    
    TeamMessageCustom::MixedTeamHeader userMsgMixedTeam;
    custom.toMixedTeamHeader(userMsgMixedTeam);

    size_t buUserSize = userMsgBU.ByteSize();
    size_t mixedUserSize = sizeof(userMsgMixedTeam);
    size_t userSize = buUserSize + mixedUserSize;
    if (userSize < SPL_STANDARD_MESSAGE_DATA_SIZE) {
        spl.numOfDataBytes = static_cast<uint16_t>(userSize);

        // 1. write custom data as mixed team header
        memcpy(spl.data, (char*)&userMsgMixedTeam, mixedUserSize);

        // 2. write custom data in BerlinUnited format
        userMsgBU.SerializeToArray(spl.data + mixedUserSize, static_cast<int>(userSize));
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

    // parse mixed team part of user data
    custom.parseFromMixedTeamHeader(spl.data, spl.numOfDataBytes);

    // parse BerlinUnited part of user data
    if(spl.numOfDataBytes > customOffset)
    {
      naothmessages::BUUserTeamMessage userData;
      try
      {
        // parse the custom data and make sure we have a valid key
        if (userData.ParseFromArray(spl.data + customOffset, static_cast<int>(spl.numOfDataBytes)-static_cast<int>(customOffset))
                && userData.key() == NAOTH_TEAMCOMM_MESAGE_KEY)
        {
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
  batteryCharge(0.0),
  temperature(0.0),
  cpuTemperature(0.0),
  whistleDetected(false),
  whistleCount(0),
  // init with "invalid" position
  teamBall(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()),
  robotState(PlayerInfo::initial),
  readyToWalk(false)
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
    << "\t" << "readyToWalk: " << (readyToWalk ? "yes" : "no") << "\n"
    << "\t" << "robotState: " << PlayerInfo::toString(robotState) << "\n"
    << "\t" << "batteryCharge: " << batteryCharge << "\n"
    << "\t" << "temperature: " << temperature << "°C\n"
    << "\t" << "CPU: " << cpuTemperature << "°C\n"
    << "\t" << "whistleDetected: " << (whistleDetected ? "yes" : "no") << "\n"
    << "\t" << "whistleCount: " << whistleCount << "\n"
    << "\t" << "ball velocity: " << std::fixed << std::setprecision(4)
            << std::setw(9) << ballVelocity.x << ", "
            << std::setw(9) << ballVelocity.y << "\n"
    << "\t" << "teamball position: "
        << teamBall.x << "/" << teamBall.y << "\n"
    << "\t" << "role: "
        << Roles::getName(robotRole.role) << " / " << Roles::getName(robotRole.dynamic)
    << "\n";
  if(!ntpRequests.empty()) {
      stream << "\t" << "ntp request for: \n";
      for(auto const& request : ntpRequests) {
          stream << "\t\t" << request.playerNumber << ", " << request.sent << " -> " << request.received << std::endl;
      }
  }
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
    DataConversion::toMessage(ballVelocity, *(userMsg.mutable_ballvelocity()));
    userMsg.set_key(key);
    userMsg.set_robotstate((naothmessages::RobotState)robotState);
    userMsg.mutable_robotrole()->set_role_static((naothmessages::RobotRoleStatic)robotRole.role);
    userMsg.mutable_robotrole()->set_role_dynamic((naothmessages::RobotRoleDynamic)robotRole.dynamic);
    userMsg.set_readytowalk(readyToWalk);

    return userMsg;
}

void TeamMessageCustom::parseFromMixedTeamHeader(const uint8_t* rawHeader, size_t headerSize)
{
  // initialize with some values so the compiler is happy
  MixedTeamHeader header = { robotState == PlayerInfo::penalized, Roles::Static::unknown };
  if(headerSize >= sizeof(MixedTeamHeader))
  {
    memcpy(&header, rawHeader, sizeof(MixedTeamHeader));
  }
  // copy the parsed data
  robotState = (header.data & 1) != 0 ? PlayerInfo::penalized : PlayerInfo::playing;
  wantsToBeStriker = (header.data & 2) != 0;
  wasStriker = wantsToBeStriker;
  // we get 0-2 from B-Human,
  // TODO: retrieve the role of the bhuman player
  /*
  if(header.role >= static_cast<uint8_t>(Roles::Static::numOfStaticRoles)) {
      robotRole.role = Roles::unknown;
  } else {
      robotRole.role = static_cast<Roles::Static>(header.role * 3);
  }
  */
  robotRole.role =  Roles::unknown;
}

void TeamMessageCustom::toMixedTeamHeader(MixedTeamHeader& header) const
{
    // copy the information into an internal struct
    header.data = robotState == PlayerInfo::penalized ? 1 : 0;
    header.data = static_cast<int8_t>(header.data | (robotRole.dynamic == Roles::striker ? 2 : 0));
    header.role = robotRole.role;
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
    if(userData.has_ballvelocity()) {
        DataConversion::fromMessage(userData.ballvelocity(),ballVelocity);
    } else {
        ballVelocity.x = 0;
        ballVelocity.y = 0;
    }

    // be aware of the old 'isPenalized' message field!
    if(userData.has_robotstate()) {
        robotState = (PlayerInfo::RobotState) userData.robotstate();
    } else if(userData.has_ispenalized() && userData.ispenalized()) {
        robotState = PlayerInfo::penalized;
    } else {
        // for the old log files, if the robot wasn't penalized
        robotState = PlayerInfo::playing;
    }

    if(userData.has_robotrole()) {
        robotRole.role = (Roles::Static) userData.robotrole().role_static();
        robotRole.dynamic = (Roles::Dynamic) userData.robotrole().role_dynamic();
    }

    readyToWalk = userData.readytowalk();
}

