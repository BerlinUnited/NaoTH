#ifndef TEAMBALLLOCATORCANOPYCLUSTER_H
#define TEAMBALLLOCATORCANOPYCLUSTER_H

#include "ModuleFramework/Module.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/TeamBallModel.h"
#include "Representations/Modeling/RobotPose.h"
#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/TeamMessageNTP.h"
#include "Representations/Modeling/TeamMessagePlayersState.h"


BEGIN_DECLARE_MODULE(TeamBallLocatorCanopyCluster)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)
  REQUIRE(RobotPose)
  REQUIRE(TeamState)
  REQUIRE(TeamMessageNTP)
  REQUIRE(TeamMessagePlayersState)

  PROVIDE(TeamBallModel)
END_DECLARE_MODULE(TeamBallLocatorCanopyCluster);


class TeamBallLocatorCanopyCluster : protected TeamBallLocatorCanopyClusterBase
{
public:
    class Parameters: public ParameterList
    {
    public:
        Parameters() : ParameterList("TeamBallLocatorCanopyCluster")
        {
            PARAMETER_REGISTER(maxBallAge) = 850; // in ms, wait at least two messages (approx.)
            PARAMETER_REGISTER(t1) = 1000; // in mm
            PARAMETER_REGISTER(t2) = 500; // in mm
            PARAMETER_REGISTER(maxTimeTbIsValidWithoutUpdate) = 2000; // in ms
            PARAMETER_REGISTER(ballsAreOnlyValidOnField) = true; // the received balls are only 'valid' if their inside the field boundries
            PARAMETER_REGISTER(enablePlayingCheck) = true; // whether the check, if a teammate is alive, active & playing should be used
            PARAMETER_REGISTER(enableNtpAdjustment) = true; // whether the ball age should be adjusted for the network latency
            syncWithConfig();
        }
        int maxBallAge;
        int t1; // loose distance
        int t2; // tight distance
        int maxTimeTbIsValidWithoutUpdate;
        bool ballsAreOnlyValidOnField;

        bool enablePlayingCheck;
        bool enableNtpAdjustment;
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
