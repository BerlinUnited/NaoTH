#ifndef _ArmCollisionDetector2018_H_
#define _ArmCollisionDetector2018_H_

#include <ModuleFramework/Module.h>

//Representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Motion/MotionStatus.h>
#include <Representations/Motion/Request/MotionRequest.h>
#include <Representations/Motion/CollisionPercept.h>
#include "Representations/Modeling/BodyState.h"


//Tools
#include <Tools/Math/ConvexHull.h>
#include "Tools/Math/Polygon.h"
#include <Tools/DataStructures/RingBufferWithSum.h>
#include <vector>
#include <string>
#include <fstream>

//Debug and Plot
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugPlot.h>
#include <Tools/Debug/DebugModify.h>
#include <Tools/Debug/DebugParameterList.h>

#include "Tools/Filters/AssymetricalBoolFilter.h"

BEGIN_DECLARE_MODULE(ArmCollisionDetector2018)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)
  PROVIDE(DebugModify)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(MotorJointData)
  REQUIRE(SensorJointData)
  REQUIRE(MotionStatus)
  REQUIRE(MotionRequest)
  REQUIRE(BodyState)

  PROVIDE(CollisionPercept)
END_DECLARE_MODULE(ArmCollisionDetector2018)

class ArmCollisionDetector2018 : private ArmCollisionDetector2018Base
{
public:
    ArmCollisionDetector2018();
    ~ArmCollisionDetector2018();
    void execute();

public:
    class Parameter : public ParameterList
    {
    public:
        Parameter() : ParameterList("ArmCollisionDetector2018")
        {
            //Entweder direkt als Point vektor
            //Oder als Pfad zur txt was vermutlich einfacher ist
            PARAMETER_REGISTER(point_configLeft) = "reference_points_cd18Left.txt";
            PARAMETER_REGISTER(point_configRight) = "reference_points_cd18Right.txt";
            PARAMETER_REGISTER(maxErrorStand) = 0.02;
            PARAMETER_REGISTER(collect) = 32;
            PARAMETER_REGISTER(armRollError) = 0.06;

            PARAMETER_REGISTER(hysteresis_g0) = 0.03;
            PARAMETER_REGISTER(hysteresis_g1) = 0.5;
            syncWithConfig();

        }

        std::string  point_configLeft;
        std::string  point_configRight;
        double maxErrorStand;
        double armRollError;
        unsigned int collect;

        double hysteresis_g0;
        double hysteresis_g1;
    } params;

private:
    //Private variablen wie zb ringbuffer zur MJD und SJD synchronisation
    RingBuffer<double, 4> jointDataBufferLeft;
    RingBuffer<double, 4> jointDataBufferRight;
    RingBufferWithSum<double, 100> collisionBufferLeft;
    RingBufferWithSum<double, 100> collisionBufferRight;
    Math::Polygon<double> refpolyL;
    Math::Polygon<double> refpolyR;

    RingBuffer<double, 4> jointDataBufferLeftRoll;
    RingBuffer<double, 4> jointDataBufferRightRoll;
    RingBufferWithSum<double, 25> collisionBufferLeftRoll;
    RingBufferWithSum<double, 25> collisionBufferRightRoll;

    AssymetricalBoolHysteresisFilter collisionFilterLeft;
    AssymetricalBoolHysteresisFilter collisionFilterRight;
};

#endif
