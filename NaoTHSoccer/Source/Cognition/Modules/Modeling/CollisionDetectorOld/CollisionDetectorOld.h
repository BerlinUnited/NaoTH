#ifndef COLLISIONDETECTOR_OLD_H
#define COLLISIONDETECTOR_OLD_H

#include <ModuleFramework/Module.h>

#include <Representations/Modeling/CollisionModel.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Tools/Debug/DebugModify.h>

BEGIN_DECLARE_MODULE(CollisionDetectorOld)
REQUIRE(FrameInfo)
REQUIRE(UltraSoundReceiveData)
REQUIRE(ButtonData)
PROVIDE(DebugModify)
PROVIDE(CollisionModel)

END_DECLARE_MODULE(CollisionDetectorOld)

class CollisionDetectorOld : public CollisionDetectorOldBase
{
public:
  CollisionDetectorOld();
  virtual ~CollisionDetectorOld();

  virtual void execute();
private:
    FrameInfo collisionStartTimeLeft;
    FrameInfo collisionStartTimeRight;
    FrameInfo lastBumpTimeLeft;
    FrameInfo lastBumpTimeRight;
    bool bumperCollisionLeft;
    bool bumperCollisionRight;
};

#endif // COLLISIONDETECTOR_OLD_H
