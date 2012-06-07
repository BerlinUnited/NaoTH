#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <ModuleFramework/Module.h>

#include <Representations/Modeling/CollisionModel.h>
#include <Representations/Infrastructure/UltraSoundData.h>
#include <Representations/Infrastructure/ButtonData.h>
#include <Representations/Infrastructure/FrameInfo.h>

BEGIN_DECLARE_MODULE(CollisionDetector)

REQUIRE(FrameInfo)
REQUIRE(UltraSoundReceiveData)
REQUIRE(ButtonData)
PROVIDE(CollisionModel)

END_DECLARE_MODULE(CollisionDetector)

class CollisionDetector : public CollisionDetectorBase
{
public:
  CollisionDetector();
  virtual ~CollisionDetector();

  virtual void execute();

};

#endif // COLLISIONDETECTOR_H
