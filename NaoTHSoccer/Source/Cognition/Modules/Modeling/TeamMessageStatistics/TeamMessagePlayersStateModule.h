#ifndef TEAMMESSAGEPLAYERISALIVEMODULE_H
#define TEAMMESSAGEPLAYERISALIVEMODULE_H

#include "ModuleFramework/Module.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Modeling/TeamMessageStatistics.h"
#include "Representations/Infrastructure/GameData.h"


BEGIN_DECLARE_MODULE(TeamMessagePlayersStateModule)
  REQUIRE(TeamState)
  REQUIRE(FrameInfo)
  REQUIRE(TeamMessageStatistics)
  REQUIRE(GameData)
  REQUIRE(PlayerInfo)

  PROVIDE(DebugParameterList)
  PROVIDE(TeamMessagePlayersState)
END_DECLARE_MODULE(TeamMessagePlayersStateModule)


class TeamMessagePlayersStateModule : public TeamMessagePlayersStateModuleBase
{
public:
    /**
     * @brief Constructor, registers the paramters for this module.
     */
    TeamMessagePlayersStateModule() {
        getDebugParameterList().add(&params);
    }

    /**
     * @brief Destructor, unregisters the paramters for this module.
     */
    virtual ~TeamMessagePlayersStateModule() {
        getDebugParameterList().remove(&params);
    }

    /**
     * @brief Selects a 'dead'-determination method based on the 'calculationMethod' parameter.
     *        Currently only 2 methods are implemented (0, 1) and a simple default method is used,
     *        if the 'calculationMethod' parameter has an invalid value.
     * @see messageIndicator(), messageLastReceived(), messageSimple()
     */
    virtual void execute() {
        // Dead or Alive
        switch (params.calculationMethod) {
            case 0:
                // determine 'dead' status based on last msg received
                calc(&TeamMessagePlayersStateModule::messageLastReceived);
                break;
            case 1:
                // calc based on msg indicator
                calc(&TeamMessagePlayersStateModule::messageIndicator);
                break;
            default:
                // if we got a message, the robot isn't 'dead'
                calc(&TeamMessagePlayersStateModule::messageSimple);
                break;
        }
        // Active
        (this->*params.activeMethod)();
        // is playing?
        (this->*params.playingMethod)();
    }

private:
    /**
     * @brief Determines the 'dead/alive' status for each player we received a message from.
     */
    void calc(bool (TeamMessagePlayersStateModule::*func)(const TeamState::Player&) const) {
        for (const auto& it: getTeamState().players) {
            getTeamMessagePlayersState().data[it.first].alive = (this->*func)(it.second);
        }
    }

    /**
     * @brief Determines the players 'dead' status based on the statistics message indicator.
     *        If the TeamMessageStatisticsModule isn't active, the 'messageLastReceived' method is used instead.
     *        Briefly summarized: if the message indicator is ...
     *          - '0.5' we expect a new message (avg. receiving time)
     *          - '0.75' the last message of the player is more than 'avg. receiving time + 25% of the variance of the receiving time' ago
     *          - '>0.9' the last message of the player is more than 'avg. receiving time + 40% of the variance of the receiving time' ago
     *
     * @param player    the player the 'dead' status should be determined for
     * @return true|false   whether the player is 'dead' or not
     */
    bool messageIndicator(const TeamState::Player& player) const {
        const auto& stats = getTeamMessageStatistics();
        if(stats.isStatisticsActive(player.number)) {
            return stats.getMessageIndicator(player.number) <= params.maxMessageReceivingIndicator;
        }
        return messageLastReceived(player);
    }

    /**
     * @brief Determines the players 'dead/alive' status based on when the last message was received.
     *        If the last message is more than 'maxTimeLastMessageReceived' ago, the players 'alive' status is set to 'false'.
     *
     * @param player    the player the 'alive' status should be determined for
     * @return true|false   whether the player is 'dead' or 'alive'
     */
    bool messageLastReceived(const TeamState::Player& player) const {
        return (naoth::NaoTime::getSystemTimeInMilliSeconds() - player.messageParsed) <= params.maxTimeLastMessageReceived;
    }

    /**
     * @brief Returns always 'true' as 'alive' status.
     *        If we received a message from this player, we assume that he isn't 'dead'.
     *
     * @param player    the player the 'alive' status should be determined for
     * @return true|false   whether the player is 'dead' or 'alive'
     */
    bool messageSimple(const TeamState::Player& player) const {
        // HACK: prevent 'unused-parameter' warning
        return &player == &player;
    }

    /**
     * @brief Determines the 'active' state of the players and sets the 'penalized' state.
     *        A player is 'active', if he's alive, playing on the field and not penalized
     *        ('ready','set','playing').
     */
    void determineActiveStates() {
        for (const auto& it: getTeamState().players) {
            auto& active = getTeamMessagePlayersState().data[it.first].active;
            active = getTeamMessagePlayersState().isAlive(it.first) && (
                        it.second.state() == PlayerInfo::ready ||
                        it.second.state() == PlayerInfo::set ||
                        it.second.state() == PlayerInfo::playing
                    );
            getTeamMessagePlayersState().data[it.first].penalized = it.second.state() == PlayerInfo::penalized;
        }
    }

    /**
     * @brief Always assume the players are active and not penalized.
     */
    void determineActiveStatesAlways()
    {
        for (const auto& it: getTeamState().players)
        {
            getTeamMessagePlayersState().data[it.first].active = true;
            getTeamMessagePlayersState().data[it.first].penalized = false;
        }
    }

    /**
     * @brief The gamecontroller sends the active penalties of all players.
     *        Since all inactive players are penalized, we can set the active state based on the gc messages 
     *        (the player must also be "alive").
     */
    void determineActiveStatesGameController()
    {
        const auto& state   = getTeamMessagePlayersState().data;
        const auto& players = getGameData().ownTeam.players;
        for (size_t index = 0; index < players.size(); ++index)
        {
            auto n = index + 1;
            if (state.find(n) != state.cend())
            {
                getTeamMessagePlayersState().data[n].active = getTeamMessagePlayersState().isAlive(n) && !players[index].isPenalized();
                getTeamMessagePlayersState().data[n].penalized = players[index].isPenalized();
            }
        }
    }

    /**
     * @brief Determines the 'playing' state of the players.
     *        A player is currently 'playing', if he's not fallen and ready to walk.
     *        There could be other indicators, like de-localised robots.
     */
    void determinePlayingStates() {
        for (const auto& it: getTeamState().players) {
            getTeamMessagePlayersState().data[it.first].playing = !it.second.fallen() && it.second.readyToWalk();
        }
    }

    /**
     * @brief Always assume a player can actively play.
     */
    void determinePlayingStatesAlways() {
        for (const auto& it: getTeamState().players) {
            // I know the state for sure only for me
            if (getPlayerInfo().playerNumber == it.first) {
                getTeamMessagePlayersState().data[it.first].playing = !it.second.fallen() && it.second.readyToWalk();
            } else {
                getTeamMessagePlayersState().data[it.first].playing = true;
            }
        }
    }

    /** The parameter struct holding the parameters. */
    class Parameters: public ParameterList
    {
    public:
        Parameters(): ParameterList("TeamMessagePlayerIsAliveModule") {
            PARAMETER_REGISTER(calculationMethod)            = 1;
            PARAMETER_REGISTER(maxTimeLastMessageReceived)   = 2000;
            PARAMETER_REGISTER(maxMessageReceivingIndicator) = 0.85;

            // default, gc, always
            PARAMETER_REGISTER(active_method, &Parameters::setActiveMethod) = "default";
            // default, always
            PARAMETER_REGISTER(playing_method, &Parameters::setPlayingMethod) = "default";
            
            // load from the file after registering all parameters
            syncWithConfig();
        }
        virtual ~Parameters() {}

        /** */
        int calculationMethod;
        /** */
        int maxTimeLastMessageReceived;
        /** */
        double maxMessageReceivingIndicator;

        std::string active_method;
        void (TeamMessagePlayersStateModule::*activeMethod)();
        void setActiveMethod(std::string method)
        {
            if(method == "default")         { activeMethod = &TeamMessagePlayersStateModule::determineActiveStates; }
            else if (method == "always")    { activeMethod = &TeamMessagePlayersStateModule::determineActiveStatesAlways; }
            else if (method == "gc")        { activeMethod = &TeamMessagePlayersStateModule::determineActiveStatesGameController; }
            else                            { activeMethod = &TeamMessagePlayersStateModule::determineActiveStates; }
        }

        std::string playing_method;
        void (TeamMessagePlayersStateModule::*playingMethod)();
        void setPlayingMethod(std::string method)
        {
            if(method == "default")         { playingMethod = &TeamMessagePlayersStateModule::determinePlayingStates; }
            else if(method == "always")     { playingMethod = &TeamMessagePlayersStateModule::determinePlayingStatesAlways; }
            else                            { playingMethod = &TeamMessagePlayersStateModule::determinePlayingStates; }
        }
   } params;
};

#endif // TEAMMESSAGEPLAYERISALIVEMODULE_H
