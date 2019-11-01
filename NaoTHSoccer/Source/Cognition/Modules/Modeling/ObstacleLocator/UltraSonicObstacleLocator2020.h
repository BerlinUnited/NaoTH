#ifndef ULTRASONIC_OBSTACLE_LOCATOR_2020_H
#define ULTRASONIC_OBSTACLE_LOCATOR_2020_H

#include <ModuleFramework/Module.h>

#include "Tools/Debug/DebugRequest.h"
//#include "Tools/Debug/DebugModify.h"
//#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugDrawings3D.h"
//#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Math/Pose3D.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"

#include "Representations/Modeling/KinematicChain.h"
//#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/ObstacleModel.h"

BEGIN_DECLARE_MODULE(UltraSonicObstacleLocator2020)
  //PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  //PROVIDE(DebugParameterList)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugDrawings3D)

  REQUIRE(FrameInfo)
  //REQUIRE(OdometryData)
  REQUIRE(UltraSoundReceiveData)
  REQUIRE(KinematicChain)


  PROVIDE(ObstacleModel)
END_DECLARE_MODULE(UltraSonicObstacleLocator2020)

class UltraSonicObstacleLocator2020 : UltraSonicObstacleLocator2020Base
{
    public:
        UltraSonicObstacleLocator2020();
        virtual ~UltraSonicObstacleLocator2020(){}

        virtual void execute();

    private:
        Pose3D leftReceiverInTorso, rightReceiverInTorso; // const?

        Vector3d leftInWorld[UltraSoundReceiveData::numOfUSEcho];
        Vector3d rightInWorld[UltraSoundReceiveData::numOfUSEcho];

        void draw() const;

        // TODO: really required?
        // used to recognize whether a new percept is avaliable
        //UltraSoundReceiveData lastValidUltraSoundReceiveData;
        //bool is_new_data_avaliable() const;
};

#endif // ULTRASONICOBSTACLELOCATOR2020_H
