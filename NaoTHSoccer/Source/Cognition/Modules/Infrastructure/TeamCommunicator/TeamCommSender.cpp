#include "TeamCommSender.h"

#include "PlatformInterface/Platform.h"
#include <Tools/NaoTime.h>

TeamCommSender::TeamCommSender()
  :lastSentTimestamp(0),
   send_interval(400)
{
  naoth::Configuration& config = naoth::Platform::getInstance().theConfiguration;
  if ( config.hasKey("teamcomm", "send_interval") )
  {
    send_interval = config.getInt("teamcomm", "send_interval");
  }

  getDebugParameterList().add(&parameters);
}

TeamCommSender::~TeamCommSender() {
  getDebugParameterList().remove(&parameters);
}


void TeamCommSender::execute()
{
    // sets the "standard" values for the spl message part
    // this must be executed here, otherwise we would receive our own messages only in the given interval!
    fillMessageBeforeSending();

    // only send data in intervals of 500ms
    if(getWifiMode().wifiEnabled && (unsigned int)getFrameInfo().getTimeSince(lastSentTimestamp) > send_interval)
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

    bool sendBallModel = getBallModel().valid;
    if(parameters.sendBallAgeDobermann) {
      sendBallModel = (getBallModel().getFrameInfoWhenBallWasSeen().getTime() > 0);
    }

    if(sendBallModel)
    {
      // here in milliseconds (conversion to seconds is in SPLStandardMessage::createSplMessage())
      msg.ballAge = getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime());
      msg.ballPosition = getBallModel().position;
      msg.custom.ballVelocity = getBallModel().speed;
    } 
    else 
    {
      // only sent these values if the ball was never seen
      msg.ballAge = -1;
      msg.ballPosition.x = std::numeric_limits<double>::infinity();
      msg.ballPosition.y = std::numeric_limits<double>::infinity();
      msg.custom.ballVelocity.x = 0;
      msg.custom.ballVelocity.y = 0;
    }

    msg.fallen = getBodyState().fall_down_state != BodyState::upright;

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
    // update teamball in teamcomm
    if(getTeamBallModel().valid) {
      getTeamMessageData().custom.teamBall = getTeamBallModel().positionOnField;
    } else {
      // set teamball in teamcomm to an invalid value
      getTeamMessageData().custom.teamBall.x = std::numeric_limits<double>::infinity();
      getTeamMessageData().custom.teamBall.y = std::numeric_limits<double>::infinity();
    }

    msg.custom.robotState = getPlayerInfo().robotState;

    // TODO: shall we put it into config?
    msg.custom.key = NAOTH_TEAMCOMM_MESAGE_KEY;
}

