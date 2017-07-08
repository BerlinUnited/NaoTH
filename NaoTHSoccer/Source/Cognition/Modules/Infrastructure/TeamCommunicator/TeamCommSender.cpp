#include "TeamCommSender.h"
#include "PlatformInterface/Platform.h"
#include <Messages/Representations.pb.h>

#include <Tools/NaoTime.h>

#include <Tools/DoberMannTime.h>

using namespace std;

TeamCommSender::TeamCommSender()
  :lastSentTimestamp(0),
   send_interval(400),
   send_doberman_header(true)
{
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  if ( config.hasKey("teamcomm", "send_interval") )
  {
    send_interval = config.getInt("teamcomm", "send_interval");
  }
  if ( config.hasKey("teamcomm", "send_doberman_header") )
  {
    send_doberman_header = config.getBool("teamcomm", "send_doberman_header");
  }
}

void TeamCommSender::execute()
{
    // sets the "standard" values for the spl message part
    // this must be executed here, otherwise we would receive our own messages only in the given interval!
    fillMessageBeforeSending();

    // only send data in intervals of 500ms
    if((unsigned int)getFrameInfo().getTimeSince(lastSentTimestamp) > send_interval)
    {
        // create the message string of the known data
        getTeamMessageDataOut().data = getTeamMessageData().createSplMessageString();
        // remember the last sending time
        lastSentTimestamp = getFrameInfo().getTime();
    } else {
        getTeamMessageDataOut().data.clear();
    }
}

void TeamCommSender::fillMessageBeforeSending() const
{
    TeamMessageData& msg = getTeamMessageData();
    msg.playerNumber = getPlayerInfo().playerNumber;
    msg.teamNumber = getPlayerInfo().teamNumber;
    msg.pose = getRobotPose();
    msg.intention = getPlayerInfo().isPlayingStriker ? 3 : (getPlayerInfo().playerNumber == 1 ? 1 : 0);
    msg.positionConfidence = 100;
    msg.sideConfidence = 100;
    if(getBallModel().valid) {
        msg.ballAge = getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime());
        msg.ballPosition = getBallModel().position;
        msg.ballVelocity = getBallModel().speed;
    } else {
        msg.ballAge = -1;
        msg.ballPosition.x = std::numeric_limits<double>::max();
        msg.ballPosition.y = std::numeric_limits<double>::max();
        msg.ballVelocity.x = 0;
        msg.ballVelocity.y = 0;
    }
    msg.fallen = getBodyState().fall_down_state != BodyState::upright;
    msg.walkingTo = getRobotPose().translation;
    msg.shootingTo = getPlayerInfo().isPlayingStriker ? getKickActionModel().expectedBallPos : getRobotPose().translation;

    // TODO: can we make it more separate?
    msg.custom.timestamp = naoth::NaoTime::getSystemTimeInMilliSeconds();
    msg.custom.wantsToBeStriker = getRoleDecisionModel().wantsToBeStriker;
    msg.custom.wasStriker = getPlayerInfo().isPlayingStriker; // intention

    msg.custom.bodyID = getRobotInfo().bodyID;
    ASSERT(getSoccerStrategy().timeToBall >= 0);
    msg.custom.timeToBall = (unsigned int)getSoccerStrategy().timeToBall;
    msg.custom.isPenalized = getPlayerInfo().robotState == PlayerInfo::penalized;
    msg.custom.batteryCharge = getBatteryData().charge;
    msg.custom.temperature = std::max(getBodyState().temperatureLeftLeg, getBodyState().temperatureRightLeg);
    msg.custom.cpuTemperature = getCpuData().temperature;
    // TODO: shall we put it into config?
    msg.custom.key = NAOTH_TEAMCOMM_MESAGE_KEY;
}

