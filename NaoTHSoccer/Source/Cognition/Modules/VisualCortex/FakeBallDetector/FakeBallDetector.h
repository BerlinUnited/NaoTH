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

public:
    struct FakeBall {
        Vector2d position;
        Vector2d velocity;
        bool     const_velocity;
    };

    void clearFakeBalls() {
        fakeBalls.clear();
    }

    void addFakeBall(const FakeBall& fb) {
        fakeBalls.push_back(fb);
    }

private:
    FrameInfo lastFrame;

    std::vector<FakeBall> fakeBalls;

    void simulateMovementOnField(double dt);
    void provideMultiBallPercept() const;
};

#endif // FAKEBALLDETECTOR_H
