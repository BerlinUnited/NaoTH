/**
* @file ArmMotionEngine.h
*
* @author <a href="mailto:guido.schillaci@informatik.hu-berlin.de">Guido Schillaci</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Declaration of class ArmMotionEngine
*/

#ifndef _ArmMotionEngine_H_
#define	_ArmMotionEngine_H_

#include <ModuleFramework/Module.h>

#include <Tools/DataStructures/ParameterList.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Motion/Engine/InverseKinematicsMotion/InverseKinematicsMotionEngine.h"

// debug
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(ArmMotionEngine)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugParameterList)

  REQUIRE(RobotInfo)
  REQUIRE(MotionRequest)
  REQUIRE(FrameInfo)
  REQUIRE(SensorJointData)

  REQUIRE(InverseKinematicsMotionEngineService)

  PROVIDE(MotorJointData)
END_DECLARE_MODULE(ArmMotionEngine)


class ArmMotionEngine: private ArmMotionEngineBase
{
public:
  ArmMotionEngine();
  virtual ~ArmMotionEngine();

  virtual void execute();

private:
  class Parameters : public ParameterList
  {
  public:
      Parameters():ParameterList("ArmMotionParams")
    {
      PARAMETER_REGISTER(maxJointSpeed) = 60;
      PARAMETER_REGISTER(armStiffness) = 0.3;

      PARAMETER_REGISTER(armsOnBack.shoulderRoll) = 10;
      PARAMETER_REGISTER(armsOnBack.shoulderPitch) = 119;
      PARAMETER_REGISTER(armsOnBack.elbowRoll) = 60;
      PARAMETER_REGISTER(armsOnBack.elbowYaw) = 25;

      syncWithConfig();
    }

    ~Parameters() {
    }

    struct ArmsOnBack {
      double shoulderRoll;
      double shoulderPitch;
      double elbowRoll;
      double elbowYaw;
    } armsOnBack;

    double maxJointSpeed;
    double armStiffness;
  };//end ArmMotionParams

  Parameters params;

private:
  naoth::FrameInfo theMotorJointFrameInfo;
  naoth::JointData theMotorJointDataOld;
  naoth::JointData theJointData;

  naoth::SensorJointData theSensorJointData;

  double max_velocity_deg_in_second;
  double max_velocity;

  void setLeftShoulderPosition(double shoulderPitch, double shoulderRoll);
  void setLeftShoulderStiffness(double shoulderPitch, double shoulderRoll);
  void setLeftElbowPosition(double elbowYaw, double elbowRoll);
  void setLeftElbowStiffness(double elbowYaw, double elbowRoll);
  void setLeftArmJointPosition(double shoulderPitch, double shoulderRoll, double elbowYaw, double elbowRoll);
  void setLeftArmJointStiffness(double shoulderPitch, double shoulderRoll, double elbowYaw, double elbowRoll);

  void setRightShoulderPosition(double shoulderPitch, double shoulderRoll);
  void setRightShoulderStiffness(double shoulderPitch, double shoulderRoll);
  void setRightElbowPosition(double elbowYaw, double elbowRoll);
  void setRightElbowStiffness(double elbowYaw, double elbowRoll);
  void setRightArmJointPosition(double shoulderPitch, double shoulderRoll, double elbowYaw, double elbowRoll);
  void setRightArmJointStiffness(double shoulderPitch, double shoulderRoll, double elbowYaw, double elbowRoll);

  void setBothArmsJointPosition(double left_shoulderPitch, double left_shoulderRoll, double left_elbowYaw, double left_elbowRoll,
                                double right_shoulderPitch, double right_shoulderRoll, double right_elbowYaw, double right_elbowRoll);

  // set all the joints with the same stiffness
  void setStiffness(double stiffness);
  void hold();

  bool armsDown();
  bool armsOnBack();
  bool armsSynchronisedWithWalk();

  bool moveToJoints(const double (&position)[JointData::numOfJoint]);

  inline InverseKinematicsMotionEngine& getEngine() const
  {
    return getInverseKinematicsMotionEngineService().getEngine();
  }
};

#endif	/* _ArmMotionEngine_H_ */

