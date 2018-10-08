#include "FreeKickModule.h"

void FreeKickModule::execute()
{
    bool isKickOffSituation = false;
    bool isOwnKickOff = false;

    if(lastSetPlay == GameData::set_none && getGameData().setPlay != GameData::set_none) {
        // is own kickoff
        if(getPlayerInfo().kickoff) {
            // TODO: ???
        } else {
            // its the opponents kickoff
            const naoth::GameData::TeamInfo& gcTeam = getGameData().ownTeam;
            // retrieve last post of the player who's responsible for the free kick
            for(unsigned int i = 0; i < getGameData().ownTeam.players.size(); ++i) {
                // TODO: i need to find the player who was penalized in this moment!
                if(getGameData().ownTeam.players[i].penalty == naoth::GameData::player_pushing) {
                    const auto& player = getTeamMessage().data.find(i+1);
                    if(player != getTeamMessage().data.cend()) {
                        player->second.pose;
                    }
                }
            }

            // TODO: calculate position of the fouling player
            // TODO: calculate position of teamball
        }
    }
}
