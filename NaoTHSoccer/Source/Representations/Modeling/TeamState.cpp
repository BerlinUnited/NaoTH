#include "TeamState.h"

#include <Representations/Infrastructure/Roles.h>
#include <Tools/DataConversion.h>
#include <Messages/TeamMessage.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

void Serializer<TeamState>::serialize(const TeamState& r, std::ostream& stream)
{
    naothmessages::TeamState state;
    for(auto const& it: r.players)
    {
        auto player = it.second;
        auto msg = state.add_players();

        msg->set_number(player.number);
        msg->mutable_messageframeinfo()->set_framenumber(player.messageFrameInfo.getFrameNumber());
        msg->mutable_messageframeinfo()->set_time(player.messageFrameInfo.getTime());
        msg->set_messageparsed(player.messageParsed);
        msg->set_messagetimestamp(player.messageTimestamp);

        // only if any sync data is set
        if(player.ntpRequests().size() > 0)
        {
            // ... set the time syncing info
            for(auto const& request: player.ntpRequests()) {
                auto msgPlayer = msg->add_ntprequest();
                msgPlayer->set_playernum(request.playerNumber);
                msgPlayer->set_sent(request.sent);
                msgPlayer->set_received(request.received);
            }
            msg->set_ntprequestupdate(player.ntpRequests.time());
        }

        msg->set_robotstate((naothmessages::RobotState)player.state());
        msg->set_robotstateupdate(player.state.time());

        msg->set_fallen(player.fallen());
        msg->set_fallenupdate(player.fallen.time());

        msg->set_readytowalk(player.readyToWalk());
        msg->set_readytowalkupdate(player.readyToWalk.time());

        DataConversion::toMessage(player.pose(), *(msg->mutable_pose()));
        msg->set_poseupdate(player.pose.time());

        msg->set_ballage((int)player.ballAge());
        msg->set_ballageupdate(player.ballAge.time());

        DataConversion::toMessage(player.ballPosition(), *(msg->mutable_ballposition()));
        msg->set_ballpositionupdate(player.ballPosition.time());

        msg->set_timetoball(player.timeToBall());
        msg->set_timetoballupdate(player.timeToBall.time());

        msg->set_wantstobestriker(player.wantsToBeStriker());
        msg->set_wantstobestrikerupdate(player.wantsToBeStriker.time());

        msg->set_wasstriker(player.wasStriker());
        msg->set_wasstrikerupdate(player.wasStriker.time());

        msg->mutable_robotrole()->set_role_static((naothmessages::RobotRoleStatic)player.robotRole().role);
        msg->mutable_robotrole()->set_role_dynamic((naothmessages::RobotRoleDynamic)player.robotRole().dynamic);
        msg->set_robotroleupdate(player.robotRole.time());
    }
}

void Serializer<TeamState>::deserialize(std::istream& stream, TeamState& r)
{
    naothmessages::TeamState state;

    google::protobuf::io::IstreamInputStream buf(&stream);
    state.ParseFromZeroCopyStream(&buf);
    for(int i=0; i < state.players_size(); i++)
    {
        const auto& msg = state.players(i);
        auto player = r.getPlayer(msg.number());

        player.messageFrameInfo.setFrameNumber(msg.messageframeinfo().framenumber());
        player.messageFrameInfo.setTime(msg.messageframeinfo().time());
        player.messageParsed = msg.messageparsed();
        player.messageTimestamp = msg.messagetimestamp();

        if(msg.ntprequest_size() > 0)
        {
            auto ntpRequests = std::vector<TeamMessageNTP::Request>(msg.ntprequest_size());
            for(int i=0; i < msg.ntprequest_size(); i++)
            {
                auto& request = msg.ntprequest((int)i);
                auto& syncingPlayer = ntpRequests[i];
                syncingPlayer.playerNumber = request.playernum();
                syncingPlayer.sent = request.sent();
                syncingPlayer.received = request.received();
            }
            player.ntpRequests = ntpRequests;
        }

        if (msg.has_robotstate()) {
            player.state = (PlayerInfo::RobotState) msg.robotstate();
        }
        
        if (msg.has_fallen()) {
            player.fallen = msg.fallen();
        }

        if (msg.has_readytowalk()) {
            player.readyToWalk = msg.readytowalk();
        }

        if (msg.has_pose()) {
            player.pose = { msg.pose().rotation(), msg.pose().translation().x(), msg.pose().translation().y() };
        }

        if (msg.has_ballage()) {
            player.ballAge = msg.ballage();
        }

        if (msg.has_ballposition()) {
            player.ballPosition = { msg.ballposition().x(), msg.ballposition().y() };
        }

        if (msg.has_timetoball()) {
            player.timeToBall = msg.timetoball();
        }

        if (msg.has_wantstobestriker()) {
            player.wantsToBeStriker = msg.wantstobestriker();
        }

        if (msg.has_wasstriker()) {
            player.wasStriker = msg.wasstriker();
        }

        if (msg.has_robotrole()) {
            player.robotRole = { (Roles::Static) msg.robotrole().role_static(), (Roles::Dynamic) msg.robotrole().role_dynamic() };
        }
    }
}