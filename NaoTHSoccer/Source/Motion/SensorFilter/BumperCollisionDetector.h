#ifndef BUMPERCOLLISIONDETECTOR_H
#define BUMPERCOLLISIONDETECTOR_H

#include <ModuleFramework/Module.h>

#include <Representations/Modeling/CollisionModel.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Tools/Debug/DebugModify.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>

BEGIN_DECLARE_MODULE(BumperCollisionDetector)
REQUIRE(FrameInfo)
REQUIRE(UltraSoundReceiveData)
REQUIRE(ButtonData)
PROVIDE(DebugModify)
PROVIDE(CollisionModel)
PROVIDE(DebugParameterList)

END_DECLARE_MODULE(BumperCollisionDetector)

class BumperCollisionDetector : public BumperCollisionDetector
{
public:
    class Parameter : public ParameterList
    {
    public:
        Parameter() : ParameterList("CollisionDetectorBumper")
        {
            PARAMETER_REGISTER(collisionInterval) = 500;
            PARAMETER_REGISTER(timesToBump) = 3;
            syncWithConfig();
        }

        double collisionInterval;
        double timesToBump;
    } params;
    BumperCollisionDetector();
    virtual ~BumperCollisionDetector();

  virtual void execute();
private:
    FrameInfo collisionStartTimeLeft;
    FrameInfo collisionStartTimeRight;
    FrameInfo lastBumpTimeLeft;
    FrameInfo lastBumpTimeRight;
    bool bumperCollisionLeft;
    bool bumperCollisionRight;
};

#endif // BumperCollisionDetector_h
