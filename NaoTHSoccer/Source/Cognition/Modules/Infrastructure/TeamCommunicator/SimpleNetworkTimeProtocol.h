#ifndef SIMPLENETWORKTIMEPROTOCOL_H
#define SIMPLENETWORKTIMEPROTOCOL_H

#include "ModuleFramework/Module.h"
#include "Tools/DataStructures/ParameterList.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/TeamMessageTimeStatistics.h"


BEGIN_DECLARE_MODULE(SimpleNetworkTimeProtocol)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugPlot)

  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamMessage)

  PROVIDE(TeamMessageTimeStatistics)
  PROVIDE(TeamMessageData)
END_DECLARE_MODULE(SimpleNetworkTimeProtocol)


class SimpleNetworkTimeProtocol : public SimpleNetworkTimeProtocolBase
{
public:
    /** Constructor */
    SimpleNetworkTimeProtocol();
    /** Destructor */
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
    /** Creates an ntp request for an randomly selected teammate. */
    void createNtpRequest();
    /** Updates the players team message statistics. */
    void updateMessageStatistics();
};

#endif // SIMPLENETWORKTIMEPROTOCOL_H
