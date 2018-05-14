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

  bool useWalk2018;
  
  struct Stand 
  {
    double speed;
    bool enableStabilization;
    bool enableStabilizationRC16;
    double stiffness;

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


  struct Walk 
  {
    struct General
    {
      double bodyPitchOffset;
      double hipOffsetX;

      double stiffness;
      double stiffnessArms;
      bool   useArm;

      // hip joint correction
      double hipRollSingleSupFactorLeft;
      double hipRollSingleSupFactorRight;
    } general;


    // hip trajectory geometry
    struct Hip
    {
      double comHeight;
      double comHeightOffset;
      double comStepOffsetY;
      double ZMPOffsetY;
      double ZMPOffsetYByCharacter;

      bool newZMP_ON;
    } hip;

    // step geometry
    struct Step
    {
      int duration;
      bool dynamicDuration;
      int doubleSupportTime;
    
      double stepHeight;
      bool splineFootTrajectory;
    } step;


    // step geometry
    struct Kick
    {
      double stepHeight;
      double ZMPOffsetY;
    } kick;
    

    // step limits
    struct Limits
    {
      double maxTurnInner;
      double maxStepTurn;
      double maxStepLength;
      double maxStepLengthBack;
      double maxStepWidth;
      double maxStepChange; // (0 - 1]

      // step control
      double maxCtrlTurn;
      double maxCtrlLength;
      double maxCtrlWidth;
    } limits;
    

    struct Stabilization
    {
      // FSR stabilizators
      //bool enableFSRProtection;
      //bool enableWaitLanding;
      //unsigned int minFSRProtectionCount;
    
      //int maxUnsupportedCount;
      //int maxWaitLandingCount; // <0 means wait for ever until landing

      double emergencyStopError;
      unsigned int maxEmergencyCounter;

      // enable stabilization by rotating the body
      bool rotationStabilize;
      bool rotationStabilizeRC16;
      bool rotationStabilizeNewIMU;

      // enable the PD-control for the feet
      bool stabilizeFeet;
      // differential and proportional factors for rotation on x- and y- axes
      Vector2d stabilizeFeetP;
      Vector2d stabilizeFeetD;

      // enable the synamic adaptation of the stepsize
      bool dynamicStepsize;
      double dynamicStepsizeP;
      double dynamicStepsizeD;

      struct HipOffsetBasedOnStepChange {
          double x;
          double y;
      } hipOffsetBasedOnStepChange;

      struct HipOffsetBasedOnStepLength {
          double x;
          double y;
      } maxHipOffsetBasedOnStepLength, maxHipOffsetBasedOnStepLengthForKicks;

      struct RotationStabilization{
          Vector2d P;
          Vector2d VelocityP;
          Vector2d D;
      } rotation, rotationRC16, rotationNewIMU;

    } stabilization;

    struct ZMP{
        struct Bezier{
            double transitionScaling;
            double inFootScalingY;
            double inFootSpacing;
            double offsetX;
            double offsetY;
            double offsetXForKicks;
            double offsetYForKicks;
        } bezier;

        struct Bezier2{
            double offsetT;
            double offsetY;
        } bezier2;
    } zmp;

  } walk;

//  struct RotationStabilize
//  {
//      Vector2d k;
//      Vector2d threshold;
//  } rotationStabilize;

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
