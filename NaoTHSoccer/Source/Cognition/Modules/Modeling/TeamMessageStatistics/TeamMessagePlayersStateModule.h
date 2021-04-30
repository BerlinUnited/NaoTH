#ifndef TEAMMESSAGEPLAYERISALIVEMODULE_H
#define TEAMMESSAGEPLAYERISALIVEMODULE_H

#include "ModuleFramework/Module.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"
#include "Representations/Modeling/TeamMessageStatistics.h"


BEGIN_DECLARE_MODULE(TeamMessagePlayersStateModule)
  REQUIRE(TeamMessage)
  REQUIRE(FrameInfo)
  REQUIRE(TeamMessageStatistics)

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
        determineActiveStates();
        // is playing?
        determinePlayingStates();
    }

private:
    /**
     * @brief Determines the 'dead/alive' status for each player we received a message from.
     */
    void calc(bool (TeamMessagePlayersStateModule::*func)(const TeamMessageData&) const) {
        for(const auto& it : getTeamMessage().data) {
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
    bool messageIndicator(const TeamMessageData& data) const {
        const auto& stats = getTeamMessageStatistics();
        if(stats.isStatisticsActive(data.playerNumber)) {
            return stats.getMessageIndicator(data.playerNumber) <= params.maxMessageReceivingIndicator;
        }
        return messageLastReceived(data);
    }

    /**
     * @brief Determines the players 'dead/alive' status based on when the last message was received.
     *        If the last message is more than 'maxTimeLastMessageReceived' ago, the players 'alive' status is set to 'false'.
     *
     * @param player    the player the 'alive' status should be determined for
     * @return true|false   whether the player is 'dead' or 'alive'
     */
    bool messageLastReceived(const TeamMessageData& data) const {
        return getFrameInfo().getTimeSince(data.frameInfo) <= params.maxTimeLastMessageReceived;
    }

    /**
     * @brief Returns always 'true' as 'alive' status.
     *        If we received a message from this player, we assume that he isn't 'dead'.
     *
     * @param player    the player the 'alive' status should be determined for
     * @return true|false   whether the player is 'dead' or 'alive'
     */
    bool messageSimple(const TeamMessageData& data) const {
        // HACK: prevent 'unused-parameter' warning
        return &data == &data;
    }

    /**
     * @brief Determines the 'active' state of the players and sets the 'penalized' state.
     *        A player is 'active', if he's alive, playing on the field and not penalized
     *        ('ready','set','playing').
     */
    void determineActiveStates() {
        for(const auto& it : getTeamMessage().data) {
            auto& active = getTeamMessagePlayersState().data[it.first].active;
            active = getTeamMessagePlayersState().isAlive(it.first) && (
                        it.second.custom.robotState == PlayerInfo::ready ||
                        it.second.custom.robotState == PlayerInfo::set ||
                        it.second.custom.robotState == PlayerInfo::playing
                    );
            getTeamMessagePlayersState().data[it.first].penalized = it.second.custom.robotState == PlayerInfo::penalized;
        }
    }

    /**
     * @brief Determines the 'playing' state of the players.
     *        A player is currently 'playing', if he's not fallen and ready to walk.
     *        There could be other indicators, like de-localised robots.
     */
    void determinePlayingStates() {
        for(const auto& it : getTeamMessage().data) {
            getTeamMessagePlayersState().data[it.first].playing = !it.second.fallen && it.second.custom.readyToWalk;
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
   } params;
};

#endif // TEAMMESSAGEPLAYERISALIVEMODULE_H
