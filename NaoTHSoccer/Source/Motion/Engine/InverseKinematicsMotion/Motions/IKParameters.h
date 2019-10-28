/**
* @file IKParameters.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
* Declaration of parameters for IK motion
*/

#ifndef _IK_PARAMETERS_H_
#define _IK_PARAMETERS_H_

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Math/Vector2.h>

class IKParameters : public ParameterList
{
public:

  double footOffsetY;

  struct Stand 
  {
    double speed;
    bool enableStabilization;
    bool enableStabilizationRC16;
    double stiffnessGotoPose;
    double stiffnessRelax;

    double bodyPitchOffset;
    double hipOffsetX;

    struct Stabilization{
        struct RotationStabilization{
            Vector2d P;
            Vector2d VelocityP;
            Vector2d D;
        } rotation, rotationRC16;
    } stabilization;

    struct Relax {

        bool   enable;
        double allowedDeviation;
        double allowedRotationDeviation;
        double timeBonusForCorrection;

        struct JointOffsetTuning {
            bool   enable;
            double deadTime;
            double currentThreshold;
            double minimalJointStep;
        } jointOffsetTuning;

        struct StiffnessControl {
            bool   enable;
//            double deadTime;
            double minAngle;
            double minStiffness;
            double maxAngle;
            double maxStiffness;
        } stiffnessControl;
    } relax;
  } stand;

  struct Arm 
  {
    // the max joint speed in degree/second
    double maxSpeed;

    // move shoulder according to interial sensor
    struct InertialModelBasedMovement {
        double shoulderPitchInterialSensorRate;
        double shoulderRollInterialSensorRate;
    } inertialModelBasedMovement;

    // move the arm according to motion/walk
    struct SynchronisedWithWalk {
        double shoulderPitchRate;
        double shoulderRollRate;
        double elbowRollRate;
    } synchronisedWithWalk;
  } arm;

  struct BalanceCoMParameter 
  {
    double kP;
    double kI;
    double kD;
    double threshold;
  } balanceCoM;

  IKParameters();

  virtual ~IKParameters();
};

#endif // __IK__PARAMETERS_H_
