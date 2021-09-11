#ifndef FAKEBALLDETECTOR_H
#define FAKEBALLDETECTOR_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"

#include "Representations/Perception/MultiBallPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"

#include "Representations/Perception/CameraMatrix.h"
#include <Tools/CameraGeometry.h>

#include <random>

BEGIN_DECLARE_MODULE(FakeBallDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)

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
        bool     const_velocity = false;
        bool     enabled_detection_noise = false;
        bool     enabled_pixel_noise = false;

        struct MultivariateNormalNoise{
            struct Dimension {
                std::default_random_engine rng;
                std::normal_distribution<double> dist;

                // required by cppyy
                void setDist(double mean, double std){
                    dist = std::normal_distribution<double>(mean, std);
                }
            } x, y;

            Vector2d operator()() {return Vector2d(x.dist(x.rng), y.dist(y.rng));}

        } top_pixel_noise, bottom_pixel_noise;

        struct BernoulliNoise{
            std::default_random_engine rng;
            std::bernoulli_distribution dist;

            bool operator()() {return dist(rng);}

            // required by cppyy
            void setDist(double mean){
                dist = std::bernoulli_distribution(mean);
            }
        } detected_in_top, detected_in_bottom;
    };

    void clearFakeBalls() {
        fakeBalls.clear();
    }

    void addFakeBall(const FakeBall& fb) {
        fakeBalls.push_back(fb);
    }

    const std::vector<FakeBall>& getFakeBalls() {
        return fakeBalls;
    }

private:
    FrameInfo lastFrame;

    std::vector<FakeBall> fakeBalls;

    void simulateMovementOnField(double dt);
    void provideMultiBallPercept();
};

#endif // FAKEBALLDETECTOR_H
