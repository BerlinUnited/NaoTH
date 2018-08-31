#ifndef TEAMMESSAGEPLAYERISALIVEMODULE_H
#define TEAMMESSAGEPLAYERISALIVEMODULE_H

#include "ModuleFramework/Module.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/TeamMessagePlayerIsAlive.h"
#include "Representations/Modeling/TeamMessageStatistics.h"


BEGIN_DECLARE_MODULE(TeamMessagePlayerIsAliveModule)
  REQUIRE(TeamMessage)
  REQUIRE(FrameInfo)
  REQUIRE(TeamMessageStatistics)

  PROVIDE(DebugParameterList)
  PROVIDE(TeamMessagePlayerIsAlive)
END_DECLARE_MODULE(TeamMessagePlayerIsAliveModule)


class TeamMessagePlayerIsAliveModule : public TeamMessagePlayerIsAliveModuleBase
{
public:
    /**
     * @brief Constructor, registers the paramters for this module.
     */
    TeamMessagePlayerIsAliveModule() {
        getDebugParameterList().add(&params);
    }

    /**
     * @brief Destructor, unregisters the paramters for this module.
     */
    virtual ~TeamMessagePlayerIsAliveModule() {
        getDebugParameterList().remove(&params);
    }

    /**
     * @brief Selects a 'dead'-determination method based on the 'calculationMethod' parameter.
     *        Currently only 2 methods are implemented (0, 1) and a simple default method is used,
     *        if the 'calculationMethod' parameter has an invalid value.
     * @see messageIndicator(), messageLastReceived(), messageSimple()
     */
    virtual void execute() {
        switch (params.calculationMethod) {
            case 0:
                // determine 'dead' status based on last msg received
                calc(&TeamMessagePlayerIsAliveModule::messageLastReceived);
                break;
            case 1:
                // calc based on msg indicator
                calc(&TeamMessagePlayerIsAliveModule::messageIndicator);
                break;
            default:
                // if we got a message, the robot isn't 'dead'
                calc(&TeamMessagePlayerIsAliveModule::messageSimple);
                break;
        }
    }

private:
    /**
     * @brief Determines the 'dead/alive' status for each player we received a message from.
     */
    void calc(bool (TeamMessagePlayerIsAliveModule::*func)(const TeamMessageData&) const) {
        for(const auto& it : getTeamMessage().data) {
            getTeamMessagePlayerIsAlive().data[it.first] = (this->*func)(it.second);
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
