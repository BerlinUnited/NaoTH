#include "TeamCommEventSender.h"

TeamCommEventSender::TeamCommEventSender() : lastSentTimestamp(0)
{
  getDebugParameterList().add(&parameters);
}

TeamCommEventSender::~TeamCommEventSender()
{
  getDebugParameterList().remove(&parameters);
}

void TeamCommEventSender::execute()
{
  // first update my own state
  updateMyself();
  // ... then send the message based on the updated state
  if (shouldSendMessage() && createMessage()) {
      // remember the last sending time
      lastSentTimestamp = getFrameInfo().getTime();

      PLOT(std::string("TeamCommEventSender:message_size"), static_cast<double>(getTeamMessageDataOut().data.size()));
  } else {
      getTeamMessageDataOut().data.clear();
  }
}

void TeamCommEventSender::updateMyself() const
{
  auto& myself = getTeamState().getPlayer(getPlayerInfo().playerNumber);

  myself.messageTimestamp = naoth::NaoTime::getSystemTimeInMilliSeconds();
  myself.ntpRequests      = getTeamMessageNTP().requests;
  myself.state            = getMotionStatus().currentMotion == motion::init ? PlayerInfo::initial : getPlayerInfo().robotState;
  myself.fallen =
      getBodyState().fall_down_state != BodyState::upright ||
      getMotionStatus().currentMotion == motion::stand_up_from_back ||
      getMotionStatus().currentMotion == motion::stand_up_from_side ||
      getMotionStatus().currentMotion == motion::stand_up_from_front ||
      getMotionStatus().currentMotion == motion::stand_up_from_back_arms_back;
  myself.readyToWalk      = getBodyState().readyToWalk;
  myself.pose             = getRobotPose();

  if (getBallModel().knows) {
      myself.ballAge      = getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime());
      myself.ballPosition = getBallModel().position;
  } else {
      myself.ballAge      = -1;
      myself.ballPosition = {std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};
  }

  // TODO: check the types of myself.timeToBall and getSoccerStrategy().timeToBall
  myself.timeToBall       = static_cast<unsigned int>(getSoccerStrategy().timeToBall);
  myself.wantsToBeStriker = getRoleDecisionModel().wantsToBeStriker;
  myself.wasStriker       = getRoleDecisionModel().isStriker(getPlayerInfo().playerNumber);
  myself.robotRole        = getRoleDecisionModel().getRole(getPlayerInfo().playerNumber);
}

inline bool TeamCommEventSender::shouldSendMessage() const
{
    return getWifiMode().wifiEnabled
      && (!parameters.strictMessageBudget || getGameData().ownTeam.messageBudget >= parameters.thresholdMessageBudget);
}

bool TeamCommEventSender::createMessage() const
{
    const auto& myself = getTeamState().getPlayer(getPlayerInfo().playerNumber);

    Message message;
    message.set_number(getPlayerInfo().playerNumber);
    message.set_messagetimestamp(naoth::NaoTime::getSystemTimeInMilliSeconds());

    bool send = false;

    if (getTeamMessageDecision().send_ntpRequests()) {
        for (const auto& request : myself.ntpRequests())
        {
          auto ntp = message.add_ntprequest();
          ntp->set_playernum(request.playerNumber);
          ntp->set_sent(request.sent);
          ntp->set_received(request.received);
        }
        send = true;
    }
    if (getTeamMessageDecision().send_state()) {
        message.set_robotstate((naothmessages::RobotState)myself.state());
        send = true;
    }
    if (getTeamMessageDecision().send_fallen()) {
        message.set_fallen(myself.fallen());
        send = true;
    }
    if (getTeamMessageDecision().send_readyToWalk()) {
        message.set_readytowalk(myself.readyToWalk());
        send = true;
    }
    if (getTeamMessageDecision().send_pose()) {
        message.mutable_pose()->mutable_translation()->set_x(myself.pose().translation.x);
        message.mutable_pose()->mutable_translation()->set_y(myself.pose().translation.y);
        message.mutable_pose()->set_rotation(myself.pose().rotation);
        send = true;
    }
    if (getTeamMessageDecision().send_ballAge()) {
        // TODO: check the type of myself.ballAge - does it have to be double?
        message.set_ballage(static_cast<std::int32_t>(myself.ballAge()));
        send = true;
    }
    if (getTeamMessageDecision().send_ballPosition()) {
      message.mutable_ballposition()->set_x(myself.ballPosition().x);
      message.mutable_ballposition()->set_y(myself.ballPosition().y);
    }
    if (getTeamMessageDecision().send_timeToBall()) {
        message.set_timetoball(myself.timeToBall());
        send = true;
    }
    if (getTeamMessageDecision().send_wantsToBeStriker()) {
        message.set_wantstobestriker(myself.wantsToBeStriker());
        send = true;
    }
    if (getTeamMessageDecision().send_wasStriker()) {
        message.set_wasstriker(myself.wasStriker());
        send = true;
    }
    if (getTeamMessageDecision().send_robotRole()) {
        message.mutable_robotrole()->set_role_static((naothmessages::RobotRoleStatic)myself.robotRole().role);
        message.mutable_robotrole()->set_role_dynamic((naothmessages::RobotRoleDynamic)myself.robotRole().dynamic);
        send = true;
    }

    // only send message if there's something to send
    if (send)
    {
      getTeamMessageDataOut().data = message.SerializeAsString();
    }
        
    return send;
}
