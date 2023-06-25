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
    if(shouldSendMessage() && createMessage())
    {
        // remember the last sending time
        lastSentTimestamp = getFrameInfo().getTime();

        PLOT(std::string("TeamCommEventSender:message_size"), static_cast<double>(getTeamMessageDataOut().data.size()));
    } else {
        getTeamMessageDataOut().data.clear();
    }
}

inline bool TeamCommEventSender::shouldSendMessage() const
{
    return getWifiMode().wifiEnabled
      && (!parameters.strictMessageBudget || getGameData().ownTeam.messageBudget >= parameters.thresholdMessageBudget);
}

bool TeamCommEventSender::createMessage() const
{
    Message message;
    message.set_number(getPlayerInfo().playerNumber);
    message.set_messagetimestamp(naoth::NaoTime::getSystemTimeInMilliSeconds());

    addNtpRequests(message);
    addRobotState(message);
    addFallen(message);
    addReadyToWalk(message);
    addPose(message);
    addBallAge(message);
    addTimeToBall(message);
    addWantsToBeStriker(message);
    addWasStriker(message);
    addRobotRole(message);

    // only send message if there's something to send (except player number & timestamp (7 bytes))
    if (message.ByteSize() > 7)
    {
      getTeamMessageDataOut().data = message.SerializeAsString();
      std::cout << "TeamEvent Size = " << getTeamMessageDataOut().data.size() << std::endl;
      return true;
    }
        
    return false;
}

void TeamCommEventSender::addNtpRequests(Message& message) const
{
  for (const auto request : getTeamMessageNTP().requests)
  {
    auto ntp = message.add_ntprequest();
    ntp->set_playernum(request.playerNumber);
    ntp->set_sent(request.sent);
    ntp->set_received(request.received);
  }
}

void TeamCommEventSender::addRobotState(Message& message) const
{
  message.set_robotstate((naothmessages::RobotState)(
      getMotionStatus().currentMotion == motion::init
          ? PlayerInfo::initial
          : getPlayerInfo().robotState));
}

void TeamCommEventSender::addFallen(Message& message) const
{
  bool fallen = getBodyState().fall_down_state != BodyState::upright
        || getMotionStatus().currentMotion == motion::stand_up_from_back
        || getMotionStatus().currentMotion == motion::stand_up_from_side
        || getMotionStatus().currentMotion == motion::stand_up_from_front
        || getMotionStatus().currentMotion == motion::stand_up_from_back_arms_back;
  message.set_fallen(fallen);
}

void TeamCommEventSender::addReadyToWalk(Message& message) const
{
  message.set_readytowalk(getBodyState().readyToWalk);
}

void TeamCommEventSender::addPose(Message& message) const
{
  message.mutable_pose()->mutable_translation()->set_x(getRobotPose().translation.x);
  message.mutable_pose()->mutable_translation()->set_y(getRobotPose().translation.y);
  message.mutable_pose()->set_rotation(getRobotPose().rotation);
}

void TeamCommEventSender::addBallAge(Message& message) const
{
  if (getBallModel().knows)
  {
    message.set_ballage(getFrameInfo().getTimeSince(getBallModel().getFrameInfoWhenBallWasSeen().getTime()));
  } else {
    message.set_ballage(-1);
  }
}

void TeamCommEventSender::addBallPosition(Message& message) const
{
  if (getBallModel().knows)
  {
    message.mutable_ballposition()->set_x(getBallModel().position.x);
    message.mutable_ballposition()->set_y(getBallModel().position.y);
  } else {
    message.mutable_ballposition()->set_x(std::numeric_limits<double>::infinity());
    message.mutable_ballposition()->set_y(std::numeric_limits<double>::infinity());
  }
}

void TeamCommEventSender::addTimeToBall(Message& message) const
{
  message.set_timetoball((unsigned int)getSoccerStrategy().timeToBall);
}

void TeamCommEventSender::addWantsToBeStriker(Message& message) const
{
  message.set_wantstobestriker(getRoleDecisionModel().wantsToBeStriker);
}

void TeamCommEventSender::addWasStriker(Message& message) const
{
  message.set_wasstriker(getRoleDecisionModel().isStriker(getPlayerInfo().playerNumber));
}

void TeamCommEventSender::addRobotRole(Message& message) const
{
  auto role = getRoleDecisionModel().getRole(getPlayerInfo().playerNumber);
  
  message.mutable_robotrole()->set_role_static((naothmessages::RobotRoleStatic)role.role);
  message.mutable_robotrole()->set_role_dynamic((naothmessages::RobotRoleDynamic)role.dynamic);
}