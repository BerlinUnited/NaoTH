#include "TeamMessage.h"

#include <Tools/DataConversion.h>
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<TeamMessage::Data>::serialize(const TeamMessage::Data& r, std::ostream& stream)
{
  naothmessages::TeamMessage msg;

  msg.set_playernum(r.playerNum);
  msg.set_team(r.team);
  DataConversion::toMessage(r.pose, *(msg.mutable_pose()));
  msg.set_ballage(r.ballAge);
  DataConversion::toMessage(r.ballPosition, *(msg.mutable_ballposition()));
  DataConversion::toMessage(r.ballVelocity, *(msg.mutable_ballvelocity()));
  msg.set_fallen(r.fallen);
  naothmessages::BUUserTeamMessage* userMsg = msg.mutable_user();

  userMsg->set_bodyid(r.bodyID);
  userMsg->set_timetoball(r.timeToBall);
  userMsg->set_wasstriker(r.wasStriker);
  userMsg->set_ispenalized(r.isPenalized);
  for(unsigned int i=0; i < r.opponents.size(); i++)
  {
    const TeamMessage::Opponent& rOpp = r.opponents[i];
    naothmessages::Opponent* opp = userMsg->add_opponents();
    opp->set_playernum(rOpp.playerNum);
    DataConversion::toMessage(rOpp.poseOnField, *(opp->mutable_poseonfield()));
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<TeamMessage::Data>::deserialize(std::istream& stream, TeamMessage::Data& r)
{
  naothmessages::TeamMessage msg;

  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  r.playerNum = msg.playernum();
  r.team = msg.team();
  DataConversion::fromMessage(msg.pose(), r.pose);
  r.ballAge = msg.ballage();
  DataConversion::fromMessage(msg.ballposition(), r.ballPosition);
  DataConversion::fromMessage(msg.ballvelocity(), r.ballVelocity);
  r.fallen = msg.fallen();
  r.bodyID = msg.user().bodyid();
  r.timeToBall = msg.user().timetoball();
  r.wasStriker = msg.user().wasstriker();
  r.isPenalized = msg.user().ispenalized();
  r.opponents = std::vector<TeamMessage::Opponent>(msg.user().opponents_size());

  for(unsigned int i=0; i < r.opponents.size(); i++)
  {
    r.opponents[i].playerNum = msg.user().opponents().Get(i).playernum();
    DataConversion::fromMessage(msg.user().opponents().Get(i).poseonfield(),
                                r.opponents[i].poseOnField);
  }
}
