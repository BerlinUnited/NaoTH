#include "TeamCommSender.h"

#include "PlatformInterface/Platform.h"
#include <Tools/NaoTime.h>
#include "Representations/Motion/Request/MotionID.h"

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

        PLOT(std::string("TeamCommSender:message_size"), static_cast<double>(getTeamMessageDataOut().data.size()));
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

    bool sendBallModel = getBallModel().knows;
    if(parameters.sendBallAgeDobermann) {
      sendBallModel = (getBallModel().getFrameInfoWhenBallWasSeen().getTime() > 0);
    }

    if(sendBallModel)
    {
      // here in milliseconds (conversion to seconds is in TeamMessageData::createSplMessage())
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

    // make sure we're not in the standup motion
    msg.fallen = getBodyState().fall_down_state != BodyState::upright
              || getMotionStatus().currentMotion == motion::stand_up_from_back
              || getMotionStatus().currentMotion == motion::stand_up_from_side
              || getMotionStatus().currentMotion == motion::stand_up_from_front
              || getMotionStatus().currentMotion == motion::stand_up_from_back_arms_back;

    // TODO: can we make it more separate?
    msg.custom.timestamp = naoth::NaoTime::getSystemTimeInMilliSeconds();
    msg.custom.ntpRequests      = getTeamMessageNTP().requests;
    msg.custom.wantsToBeStriker = getRoleDecisionModel().wantsToBeStriker;
    msg.custom.wasStriker = getRoleDecisionModel().isStriker(getPlayerInfo().playerNumber);

    msg.custom.bodyID = getRobotInfo().bodyID;
    ASSERT(getSoccerStrategy().timeToBall >= 0);
    msg.custom.timeToBall = (unsigned int)getSoccerStrategy().timeToBall;
    msg.custom.batteryCharge = getBatteryData().charge;
    msg.custom.temperature = std::max(getBodyState().temperatureLeftLeg, getBodyState().temperatureRightLeg);
    msg.custom.cpuTemperature = getCpuData().temperature;

    msg.custom.whistleDetected = getWhistlePercept().whistleDetected;

    // update teamball in teamcomm
    if(getTeamBallModel().valid) {
      msg.custom.teamBall = getTeamBallModel().positionOnField;
    } else {
      // set teamball in teamcomm to an invalid value
      msg.custom.teamBall.x = std::numeric_limits<double>::infinity();
      msg.custom.teamBall.y = std::numeric_limits<double>::infinity();
    }

    // if the robot is in the "inital pose" (InitialMotion) just send 'initial' otherwise send the current robotState
    msg.custom.robotState = getMotionStatus().currentMotion == motion::init ? PlayerInfo::initial : getPlayerInfo().robotState;
    msg.custom.robotRole = getRoleDecisionModel().getRole(getPlayerInfo().playerNumber);

    msg.custom.readyToWalk = getBodyState().readyToWalk;

    // TODO: shall we put it into config?
    msg.custom.key = NAOTH_TEAMCOMM_MESAGE_KEY;
}

