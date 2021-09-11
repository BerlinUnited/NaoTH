#ifndef SIMPLENETWORKTIMEPROTOCOL_H
#define SIMPLENETWORKTIMEPROTOCOL_H

#include <random>

#include "ModuleFramework/Module.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/TeamMessageNTP.h"


BEGIN_DECLARE_MODULE(SimpleNetworkTimeProtocol)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamMessage)

  PROVIDE(TeamMessageNTP)
  PROVIDE(TeamMessageData)
END_DECLARE_MODULE(SimpleNetworkTimeProtocol)


class SimpleNetworkTimeProtocol : public SimpleNetworkTimeProtocolBase
{
public:
    /** Constructor: registers debug parameters */
    SimpleNetworkTimeProtocol();
    /** Destructor: unregisters debug parameters */
    virtual ~SimpleNetworkTimeProtocol();
    /** Module execution method. */
    virtual void execute();

protected:
    /** parameters for the module */
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("SimpleNetworkTimeProtocol") {
            PARAMETER_REGISTER(maxSyncingPlayer) = 11;
            syncWithConfig();
        }
        /** max. number of players in one ntp request. */
        unsigned int maxSyncingPlayer;
    } params;
private:
    
    /** Creates an ntp request for randomly selected teammates. */
    void createNtpRequest();
    /** Updates the players team message statistics. */
    void updateMessageStatistics();

    std::random_device rd;
    std::mt19937 rng = std::mt19937(rd());
};

#endif // SIMPLENETWORKTIMEPROTOCOL_H
