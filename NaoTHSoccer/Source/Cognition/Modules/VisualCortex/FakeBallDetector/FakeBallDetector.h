#ifndef FAKEBALLDETECTOR_H
#define FAKEBALLDETECTOR_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"

#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Perception/CameraMatrix.h"
#include <Tools/CameraGeometry.h>


BEGIN_DECLARE_MODULE(FakeBallDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)

  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)

  REQUIRE(CameraInfo)
  REQUIRE(CameraMatrix)

  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrixTop)

  PROVIDE(MultiBallPercept)
END_DECLARE_MODULE(FakeBallDetector)


class FakeBallDetector: private FakeBallDetectorBase
{
public:
    FakeBallDetector();
    ~FakeBallDetector(){}

    virtual void execute();

private:
    FrameInfo lastFrame;

    struct FakeBall {
        Vector2d position;
        Vector2d velocity;
        bool     const_velocity;
    };

    std::vector<FakeBall> fakeBalls;

    void clearFakeBalls() {
        fakeBalls.clear();
    }

    void simulateMovementOnField(double dt);
    void provideMultiBallPercept() const;
};

#endif // FAKEBALLDETECTOR_H
