#ifndef TEAMBALLLOCATORCANOPYCLUSTER_H
#define TEAMBALLLOCATORCANOPYCLUSTER_H

#include "ModuleFramework/Module.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Modeling/TeamMessage.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/TeamMessageNTP.h"


BEGIN_DECLARE_MODULE(TeamBallLocatorCanopyCluster)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(TeamMessage)
  REQUIRE(RobotPose)
  REQUIRE(TeamMessageNTP)

  PROVIDE(TeamBallModel)
END_DECLARE_MODULE(TeamBallLocatorCanopyCluster)


class TeamBallLocatorCanopyCluster : protected TeamBallLocatorCanopyClusterBase
{
public:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("TeamBallLocatorCanopyClusterParameters")
        {
            PARAMETER_REGISTER(maxBallAge) = 850; // in ms, wait at least two messages (approx.)
            PARAMETER_REGISTER(t1) = 1000; // in mm
            PARAMETER_REGISTER(t2) = 500; // in mm
            PARAMETER_REGISTER(maxTimeTbIsValidWithoutUpdate) = 2000; // in ms
            syncWithConfig();
        }
        int maxBallAge;
        int t1; // loose distance
        int t2; // tight distance
        int maxTimeTbIsValidWithoutUpdate;
    } params;

    TeamBallLocatorCanopyCluster();
    ~TeamBallLocatorCanopyCluster();

    virtual void execute();

private:
    std::map<unsigned int, unsigned int> lastMessages;

    struct Ball
    {
        Vector2d pos;
        Vector2d sum;
        unsigned int size = 0;
        bool valid = true;

        Ball(const Vector2d& p) {
            pos = p;
            sum = p;
            ++size;
        }

        void add(const Ball& p) {
            sum += p.pos;
            ++size;
        }

        Vector2d center() {
            return sum / size;
        }
    };
};

#endif // TEAMBALLLOCATORCANOPYCLUSTER_H
