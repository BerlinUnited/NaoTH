#include "TeamMessage.h"

#include <Tools/DataConversion.h>
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<TeamMessage>::serialize(const TeamMessage& r, std::ostream& stream)
{
  naothmessages::TeamMessage collection;

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator it=r.data.begin();
      it != r.data.end(); ++it)
  {
    const TeamMessage::Data& d = it->second;
    naothmessages::TeamMessage::Data* msg = collection.add_data();

    msg->set_playernum(d.playerNum);
    msg->set_teamnumber(d.teamNumber);
    DataConversion::toMessage(d.pose, *(msg->mutable_pose()));
    msg->set_ballage(d.ballAge);
    DataConversion::toMessage(d.ballPosition, *(msg->mutable_ballposition()));
    DataConversion::toMessage(d.ballVelocity, *(msg->mutable_ballvelocity()));
    msg->set_fallen(d.fallen);
    naothmessages::BUUserTeamMessage* userMsg = msg->mutable_user();

    userMsg->set_timestamp(d.timestamp);
    userMsg->set_bodyid(d.bodyID);
    userMsg->set_timetoball(d.timeToBall);
    userMsg->set_wasstriker(d.wasStriker);
    userMsg->set_wantstobestriker(d.wantsToBeStriker);
    userMsg->set_ispenalized(d.isPenalized);
    userMsg->set_batterycharge(d.batteryCharge);
    userMsg->set_temperature(d.temperature);
    for(size_t i=0; i < d.opponents.size(); i++)
    {
      const TeamMessage::Opponent& rOpp = d.opponents[i];
      naothmessages::Opponent* opp = userMsg->add_opponents();
      opp->set_playernum(rOpp.playerNum);
      DataConversion::toMessage(rOpp.poseOnField, *(opp->mutable_poseonfield()));
    }

    msg->mutable_frameinfo()->set_framenumber(d.frameInfo.getFrameNumber());
    msg->mutable_frameinfo()->set_time(d.frameInfo.getTime());


  } // end for each team message data

  google::protobuf::io::OstreamOutputStream buf(&stream);
  collection.SerializeToZeroCopyStream(&buf);
}

void Serializer<TeamMessage>::deserialize(std::istream& stream, TeamMessage& r)
{
  naothmessages::TeamMessage collection;

  google::protobuf::io::IstreamInputStream buf(&stream);
  collection.ParseFromZeroCopyStream(&buf);

  for(int i=0; i < collection.data_size(); i++)
  {
    const naothmessages::TeamMessage::Data& msg = collection.data(i);

    TeamMessage::Data d;

    d.playerNum = msg.playernum();
    if(msg.has_teamnumber()) {
      d.teamNumber = msg.teamnumber();
    } else if(msg.user().has_teamnumber()) {
      d.teamNumber = msg.user().teamnumber();
    }

    DataConversion::fromMessage(msg.pose(), d.pose);
    d.ballAge = msg.ballage();
    DataConversion::fromMessage(msg.ballposition(), d.ballPosition);
    DataConversion::fromMessage(msg.ballvelocity(), d.ballVelocity);
    d.fallen = msg.fallen();
    d.bodyID = msg.user().bodyid();
    if(msg.user().has_timetoball())
    {
      d.timeToBall = msg.user().timetoball();
    }
    else
    {
      d.timeToBall = std::numeric_limits<unsigned int>::max();
    }
    d.timestamp = msg.user().timestamp();
    d.wasStriker = msg.user().wasstriker();
    d.wantsToBeStriker = msg.user().wantstobestriker();
    d.isPenalized = msg.user().ispenalized();
    d.batteryCharge = msg.user().batterycharge();
    d.temperature = msg.user().temperature();

    d.frameInfo.setFrameNumber(msg.frameinfo().framenumber());
    d.frameInfo.setTime(msg.frameinfo().time());

    d.opponents = std::vector<TeamMessage::Opponent>(msg.user().opponents_size());

    for(size_t i=0; i < d.opponents.size(); i++)
    {
      d.opponents[i].playerNum = msg.user().opponents().Get(i).playernum();
      DataConversion::fromMessage(msg.user().opponents().Get(i).poseonfield(),
                                  d.opponents[i].poseOnField);
    }

    // add the single team message data to the collection
    r.data[d.playerNum] = d;

  } // end for each team message data
}
