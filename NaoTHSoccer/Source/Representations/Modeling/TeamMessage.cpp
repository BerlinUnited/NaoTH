#include "TeamMessage.h"

#include <Tools/DataConversion.h>
#include <Messages/TeamMessage.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<TeamMessage>::serialize(const TeamMessage& r, std::ostream& stream)
{
  naothmessages::TeamMessage collection;

  // add message drops
  naothmessages::Drops *d = collection.mutable_messagedrop();
  d->set_dropnosplmessage(r.dropNoSplMessage);
  d->set_dropnotourteam(r.dropNotOurTeam);
  d->set_dropnotparseable(r.dropNotParseable);
  d->set_dropkeyfail(r.dropKeyFail);
  d->set_dropmonotonic(r.dropMonotonic);

  for(auto const &it : r.data)
  {
    const TeamMessageData& d = it.second;
    naothmessages::TeamMessage::Data* msg = collection.add_data();

    msg->set_playernum(d.playerNumber);
    msg->set_teamnumber(d.teamNumber);
    DataConversion::toMessage(d.pose, *(msg->mutable_pose()));
    msg->set_ballage((int)d.ballAge);
    DataConversion::toMessage(d.ballPosition, *(msg->mutable_ballposition()));
    msg->set_fallen(d.fallen);
    msg->mutable_user()->CopyFrom(d.custom.toProto());
    msg->mutable_frameinfo()->set_framenumber(d.frameInfo.getFrameNumber());
    msg->mutable_frameinfo()->set_time(d.frameInfo.getTime());
    /*
    // INFO: this is currently not used!
    // TODO: do we need this out of legacy reasons?!?
    for(unsigned int i=0; i < d.opponents.size(); i++)
    {
      const TeamMessage::Opponent& rOpp = d.opponents[i];
      naothmessages::Opponent* opp = userMsg->add_opponents();
      opp->set_playernum(rOpp.playerNum);
      DataConversion::toMessage(rOpp.poseOnField, *(opp->mutable_poseonfield()));
    }
    */
  } // end for each team message data

  google::protobuf::io::OstreamOutputStream buf(&stream);
  collection.SerializeToZeroCopyStream(&buf);
}

void Serializer<TeamMessage>::deserialize(std::istream& stream, TeamMessage& r)
{
  naothmessages::TeamMessage collection;

  google::protobuf::io::IstreamInputStream buf(&stream);
  collection.ParseFromZeroCopyStream(&buf);

  // set message drop stats, if available
  if(collection.has_messagedrop()) {
      naothmessages::Drops d = collection.messagedrop();
      r.dropNoSplMessage = d.has_dropnosplmessage() ? d.dropnosplmessage() : 0;
      r.dropNotOurTeam   = d.has_dropnotourteam()   ? d.dropnotourteam()   : 0;
      r.dropNotParseable = d.has_dropnotparseable() ? d.dropnotparseable() : 0;
      r.dropKeyFail      = d.has_dropkeyfail()      ? d.dropkeyfail()      : 0;
      r.dropMonotonic    = d.has_dropmonotonic()    ? d.dropmonotonic()    : 0;
  }

  for(int i=0; i < collection.data_size(); i++)
  {
    const naothmessages::TeamMessage::Data& msg = collection.data(i);

    TeamMessageData d;

    d.playerNumber = msg.playernum();
    if(msg.has_teamnumber()) {
      d.teamNumber = msg.teamnumber();
    } else if(msg.user().has_teamnumber()) {
      d.teamNumber = msg.user().teamnumber();
    }

    DataConversion::fromMessage(msg.pose(), d.pose);
    d.ballAge = msg.ballage();
    DataConversion::fromMessage(msg.ballposition(), d.ballPosition);
    d.fallen = msg.fallen();

    d.custom.parseFromProto(msg.user());

    d.frameInfo.setFrameNumber(msg.frameinfo().framenumber());
    d.frameInfo.setTime(msg.frameinfo().time());
    /*
    // INFO: this is currently not used!
    // TODO: do we need this out of legacy reasons?!?
    d.opponents = std::vector<TeamMessage::Opponent>(msg.user().opponents_size());

    for(unsigned int i=0; i < d.opponents.size(); i++)
    {
      int j = static_cast<int>(i);
      d.opponents[j].playerNum = msg.user().opponents().Get(j).playernum();
      DataConversion::fromMessage(msg.user().opponents().Get(j).poseonfield(),
                                  d.opponents[j].poseOnField);
    }
    */
    // add the single team message data to the collection
    r.data[d.playerNumber] = d;

  } // end for each team message data
}
