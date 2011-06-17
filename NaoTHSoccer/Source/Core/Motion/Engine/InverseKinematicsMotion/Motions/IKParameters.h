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
    double comHeight;
    double bodyPitchOffset;
    
    double singleSupportTime;
    double doubleSupportTime;
    
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
  
  IKParameters();
};

#endif // __IK__PARAMETERS_H_
