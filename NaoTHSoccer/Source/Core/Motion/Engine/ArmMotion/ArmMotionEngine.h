/**
* @file ArmMotionEngine.h
*
* @author <a href="mailto:guido.schillaci@informatik.hu-berlin.de">Guido Schillaci</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Declaration of class ArmMotionEngine
*/

#ifndef _ArmMotionEngine_H
#define	_ArmMotionEngine_H

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/RobotInfo.h>
//#include <Representations/Infrastructure/CameraInfo.h>
//#include "Representations/Modeling/InertialModel.h"
//#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Motion/Request/MotionRequest.h"
#include <Representations/Infrastructure/JointData.h>
//#include <Representations/Perception/CameraMatrix.h>
//#include "Representations/Motion/MotionStatus.h"

BEGIN_DECLARE_MODULE(ArmMotionEngine)
  REQUIRE(RobotInfo)
  //REQUIRE(CameraInfo)
  //REQUIRE(InertialModel)
  //REQUIRE(KinematicChainSensor)
  REQUIRE(MotionRequest)
  REQUIRE(SensorJointData)
  //REQUIRE(CameraMatrix)

  //PROVIDE(MotionStatus)
  PROVIDE(MotorJointData)
END_DECLARE_MODULE(ArmMotionEngine)


class ArmMotionEngine: private ArmMotionEngineBase
{
public:
  /** constructor */
  ArmMotionEngine();
//***  ArmMotionEngine(MotionBlackBoard& theBlackBoard);
  ~ArmMotionEngine(){};

  void execute();

private:
  //const MotionBlackBoard& theBlackBoard;

  naoth::MotorJointData theMotorJointData;
  naoth::JointData theJointData;

  naoth::SensorJointData theSensorJointData;

  //MotionStatus& theMotionStatus;
  // KinematicChain theKinematicChain;


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

};

#endif	/* _ArmMotionEngine_H */

