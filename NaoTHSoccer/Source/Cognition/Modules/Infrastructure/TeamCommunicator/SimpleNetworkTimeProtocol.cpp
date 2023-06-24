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
    // get all players (except myself)
    std::vector<unsigned int> players;
    for(auto const& it: getTeamState().players) {
        if(it.second.number == getPlayerInfo().playerNumber) { continue; }
        players.push_back(it.first);
    }

    // clear requests from previous round
    getTeamMessageNTP().requests.clear();

    // if another player is present
    if(!players.empty() && params.maxSyncingPlayer > 0) {
        // randomly select teammates
        std::shuffle(players.begin(), players.end(), SimpleNetworkTimeProtocol::rng);
        // fill ntp request for max. syncing partners
        for(unsigned int i = 0; i<players.size() && i<params.maxSyncingPlayer; ++i) {
            auto& player = getTeamState().getPlayer(players.at(i));
            getTeamMessageNTP().requests.push_back(
                {player.number, player.messageTimestamp, player.ntpRequests.time()}
            );
        }
    }
}

void SimpleNetworkTimeProtocol::updateMessageStatistics()
{
    // iterate over all team messages
    for(const auto& it : getTeamState().players)
    {
        auto player = it.second;
        // skip my own messages
        if(player.number == getPlayerInfo().playerNumber) {
            // update myself
            getTeamMessageNTP().getPlayer(player.number).lastNtpUpdate = player.ntpRequests.time();
            continue;
        }
        TeamMessageNTP::Player& ntp = getTeamMessageNTP().getPlayer(player.number);
        // Update only with newer information
        if(player.ntpRequests.time() > ntp.lastNtpUpdate)
        {
            // search for myself in teammates response messages
            auto response = std::find_if(
                        player.ntpRequests().cbegin(),
                        player.ntpRequests().cend(),
                        [&](const TeamMessageNTP::Request& r) {return r.playerNumber == getPlayerInfo().playerNumber;}
            );
            // got my 'new' synchronization message from teammate back ...
            if (response != player.ntpRequests().cend())
            {
                const auto& t1 = response->sent;
                const auto& t2 = response->received;
                const auto& t3 = player.messageTimestamp;
                const auto& t4 = player.ntpRequests.time();
                const long long rtt = (t4 - t1) - (t3 - t2);
                const long long lat = rtt / 2;
                if(ntp.rtt == 0 || ntp.rtt > rtt) {
                    ntp.rtt = rtt;
                    ntp.latency = lat;
                    ntp.offset = (t4 - lat) - t3;
                }
            }
            // set last update time
            ntp.lastNtpUpdate = player.ntpRequests.time();
        }
    }
}
