/**
* @file IKParameters.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of parameters for IK motion
*/

#ifndef _IK_PARAMETERS_H_
#define _IK_PARAMETERS_H_

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Math/Vector2.h>

class IKParameters : public ParameterList
{
public:
  double bodyPitchOffset;
  double hipOffsetX;
  double footOffsetY;
  
  struct Stand {
    double speed;
    bool enableStabilization;
  } stand;

  struct Walk {
    double stiffness;

    // hip trajectory geometry
    double comHeight;
    double ZMPOffsetY;
    double ZMPOffsetYByCharacter;
    
    // step geometry
    double singleSupportTime;
    double doubleSupportTime;
    double maxExtendDoubleSupportTime;
    double extendDoubleSupportTimeByCharacter;
    
    double stepHeight;
    //double curveFactor;
    
    // step parameters
    double maxTurnInner;
    double maxStepTurn;
    double maxStepLength;
    double maxStepLengthBack;
    double maxStepWidth;
    double maxStepChange; // (0 - 1]
    
    // FSR stabilizators
    bool enableFSRProtection;
    bool enableWaitLanding;
    unsigned int minFSRProtectionCount;
    
    int maxUnsupportedCount;
    int maxWaitLandingCount; // <0 means wait for ever until landing

    double leftHipRollSingleSupFactor;
    double rightHipRollSingleSupFactor;

    bool rotationStabilize;

    // enable the PD-control for the feet
    bool stabilizeFeet;
    // differential and proportional factors for rotation on x- and y- axes
    Vector2<double> stabilizeFeetP;
    Vector2<double> stabilizeFeetD;

    // enable the synamic adaptation of the stepsize
    bool dynamicStepsize;

    bool useArm;
  } walk;

  struct RotationStabilize {
      Vector2<double> k;
      Vector2<double> threshold;
  } rotationStabilize;

  struct Arm {
    // move shoulder according to interial sensor
    double shoulderPitchInterialSensorRate;
    double shoulderRollInterialSensorRate;

    // the max joint speed in degree/second
    double maxSpeed;
    bool alwaysEnabled;
    bool kickEnabled;
    bool walkEnabled;
  } arm;

  struct KickParameters {
    // 
    double shiftSpeed;

    //
    double time_for_first_preparation;

    //
    double retractionSpeed;

    //
    double takeBackSpeed;

    //
    double timeToLand;

    //
    double stabilization_time;

    // prolongate the kick to make it srtonger or weaker
    double strengthOffset;

    // minimal number of cycles for the preparation
    double minNumberOfPreKickSteps;
    // duration of the shifting to one foot
    double shiftTime;
    // duration of the shifting to both feets after the kick
    double stopTime;
    // duration of the kicking phase
    double kickSpeed;
    // wait a little before taking the foot back after the kick is executed
    double afterKickTime;
    // offset of the hip height according to the default one
    double hipHeightOffset;

    //
    double footRadius;
    //
    double maxDistanceToRetract;

    bool enableStaticStabilizer;


    double basicXRotationOffset;
    double extendedXRotationOffset;
    double rotationTime;

    double knee_pitch_offset;
    double ankle_roll_offset;

    // 
    double shiftOffsetYLeft;
    double shiftOffsetYRight;
  } kick;
  
  struct BalanceCoMParameter {
    double kP;
    double kI;
    double kD;
    double threshold;
  } balanceCoM;

  IKParameters();

  virtual ~IKParameters();
};

#endif // __IK__PARAMETERS_H_
