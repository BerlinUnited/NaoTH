#include "FreeKickModule.h"

void FreeKickModule::execute()
{
    // a free kick for pushing
    if(lastSetPlay == GameData::set_none && getGameData().setPlay == GameData::pushing_free_kick) {
        // is own kickoff
        if(getPlayerInfo().kickoff) {
            // TODO: ???
        } else {
            // its the opponents kickoff


            // TODO: calculate position of the fouling player
            // TODO: calculate position of teamball
        }

    }

    // update penalties of own teammates
    for(unsigned int i = 0; i < getGameData().ownTeam.players.size(); ++i) {
        penalties[i+1] = getGameData().ownTeam.players[i].penalty;
    }
}


Vector2d FreeKickModule::getByLastKnownPose() {
    // retrieve last post of the player who's responsible for the free kick
    for(unsigned int i = 0; i < getGameData().ownTeam.players.size(); ++i) {
        // player is now penalized, but wasn't before
        if(getGameData().ownTeam.players[i].penalty == naoth::GameData::player_pushing && penalties[i+1] == naoth::GameData::penalty_none) {
            // find his last position from his message
            const auto& player = getTeamMessage().data.find(i+1);
            if(player != getTeamMessage().data.cend()) {
                return player->second.pose.translation;
            }
        }
    }
    // it should always return in the loop, but as fallback return 0,0
    return Vector2d();
}
