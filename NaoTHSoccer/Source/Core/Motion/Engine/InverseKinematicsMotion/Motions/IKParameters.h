/**
* @file IKParameters.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of parameters for IK motion
*/

#ifndef __IK__PARAMETERS_H_
#define __IK__PARAMETERS_H_

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
  } stand;

  struct Walk {
    double stiffness;
    double comHeight;
    double ZMPOffsetY;
    double ZMPOffsetYByCharacter;
    
    double singleSupportTime;
    double doubleSupportTime;
    double maxExtendDoubleSupportTime;
    double extendDoubleSupportTimeByCharacter;
    
    double stepHeight;
    double curveFactor;
    
    double maxTurnInner;
    double maxStepTurn;
    double maxStepLength;
    double maxStepLengthBack;
    double maxStepWidth;
    double maxStepChange; // (0 - 1]
    
    bool enableFSRProtection;
    
    int maxUnsupportedCount;
    int maxWaitLandingCount; // <0 means wait for ever until landing

    double leftHipRollSingleSupFactor;
    double rightHipRollSingleSupFactor;
  } walk;

  struct RotationStabilize {
      Vector2<double> k;
      Vector2<double> threshold;
  } rotationStabilize;

  struct Arm {
    double maxSpeed;
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
  } kick;
  
  IKParameters();

  virtual ~IKParameters();
};

#endif // __IK__PARAMETERS_H_
