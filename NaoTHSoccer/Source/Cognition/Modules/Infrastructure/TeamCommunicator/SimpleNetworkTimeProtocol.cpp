#include "SimpleNetworkTimeProtocol.h"

SimpleNetworkTimeProtocol::SimpleNetworkTimeProtocol() {
    // expose parameters to "debug requests"
    getDebugParameterList().add(&params);
}

SimpleNetworkTimeProtocol::~SimpleNetworkTimeProtocol() {
    // remove from "debug requests"
    getDebugParameterList().remove(&params);
}

void SimpleNetworkTimeProtocol::execute() {
    // updates the time offsets (if new offsets available)
    updateMessageStatistics();
    // create sync request for teammate (random)
    createNtpRequest();
}

void SimpleNetworkTimeProtocol::createNtpRequest()
{
    TeamMessage const& tm = getTeamMessage();
    // get all players (except myself)
    std::vector<unsigned int> players;
    for(auto const& it: tm.data) {
        if(it.second.playerNumber == getPlayerInfo().playerNumber) { continue; }
        players.push_back(it.first);
    }

    TeamMessageData& msg = getTeamMessageData();
    // clear requests from previous round
    msg.custom.ntpRequests.clear();

    // if another player is present
    if(!players.empty() && params.maxSyncingPlayer > 0) {
        // randomly select teammates
        std::shuffle(players.begin(), players.end(), SimpleNetworkTimeProtocol::rng);
        // fill ntp request for max. syncing partners
        for(unsigned int i = 0; i<players.size() && i<params.maxSyncingPlayer; ++i) {
            auto& player = tm.data.at(players.at(i));
            msg.custom.ntpRequests.push_back(
                {player.playerNumber, player.custom.timestamp, player.timestampParsed}
            );
        }
    }
}

void SimpleNetworkTimeProtocol::updateMessageStatistics()
{
    // iterate over all team messages
    for(const auto& it : getTeamMessage().data)
    {
        const TeamMessageData& data = it.second;
        // skip my own messages
        if(data.playerNumber == getPlayerInfo().playerNumber) {
            // update myself
            getTeamMessageNTP().getPlayer(data.playerNumber).lastNtpUpdate = data.frameInfo;
            continue;
        }
        TeamMessageNTP::Player& player = getTeamMessageNTP().getPlayer(data.playerNumber);
        // Update only with newer information
        if(data.frameInfo > player.lastNtpUpdate)
        {
            // search for myself in teammates response messages
            auto response = std::find_if(
                        data.custom.ntpRequests.cbegin(),
                        data.custom.ntpRequests.cend(),
                        [&](const NtpRequest& r) {return r.playerNumber == getPlayerInfo().playerNumber;}
            );
            // got my 'new' synchronization message from teammate back ...
            if (response != data.custom.ntpRequests.cend())
            {
                const auto& t1 = response->sent;
                const auto& t2 = response->received;
                const auto& t3 = data.custom.timestamp;
                const auto& t4 = data.timestampParsed;
                const long long rtt = (t4 - t1) - (t3 - t2);
                const long long lat = rtt / 2;
                if(player.rtt == 0 || player.rtt > rtt) {
                    player.rtt = rtt;
                    player.latency = lat;
                    player.offset = (t4 - lat) - t3;
                }
            }
            // set last update time
            player.lastNtpUpdate = data.frameInfo;
        }
    }
}
