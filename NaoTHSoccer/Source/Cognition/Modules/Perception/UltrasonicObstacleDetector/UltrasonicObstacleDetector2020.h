#ifndef ULTRASONIC_OBSTACLE_DETECTOR_2020_H
#define ULTRASONIC_OBSTACLE_DETECTOR_2020_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Math/Pose3D.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"

#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/ObstaclePercept.h"

BEGIN_DECLARE_MODULE(UltrasonicDetector2020)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings3D)

  REQUIRE(FrameInfo)
  REQUIRE(UltraSoundReceiveData)
  REQUIRE(KinematicChain)

  PROVIDE(UltrasonicObstaclePercept)
END_DECLARE_MODULE(UltrasonicDetector2020)

class UltrasonicDetector2020 : UltrasonicDetector2020Base
{
    public:
        UltrasonicDetector2020();
        virtual ~UltrasonicDetector2020(){
            getDebugParameterList().remove(&parameter);
        }

        virtual void execute();

    private:
        Pose3D leftReceiverInTorso, rightReceiverInTorso; // const?

        std::vector<Vector3d> leftInWorld, rightInWorld;

        void draw() const;

        class UltrasonicParameter : public ParameterList {
            public:
                UltrasonicParameter() : ParameterList("UltrasonicDetector2020"){
                    PARAMETER_REGISTER(ground_threshold) = 40;

                    syncWithConfig();
                }

                double ground_threshold;
        } parameter;

        // TODO: really required?
        // used to recognize whether a new percept is avaliable
        //UltraSoundReceiveData lastValidUltraSoundReceiveData;
        //bool is_new_data_avaliable() const;
};

#endif // ULTRASONIC_OBSTACLE_DETECTOR_2020_H
