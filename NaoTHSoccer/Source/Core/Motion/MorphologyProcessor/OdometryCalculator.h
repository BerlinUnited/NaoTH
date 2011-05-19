/**
* @file OdometryCalculator.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* OdometryCalculator: calculate the odometry according to morphology and support foot
*/

#ifndef _ODOMETRY_CALCULATOR_H_
#define _ODOMETRY_CALCULATOR_H_

#include <Tools/Math/Pose3D.h>
#include <Representations/Infrastructure/FSRData.h>

#include "Core/Representations/Modeling/OdometryData.h"
#include "Core/Representations/Modeling/KinematicChain.h"


class OdometryCalculator
{
public:

  OdometryCalculator();
  ~OdometryCalculator();

  void calculateOdometry(OdometryData& od, const KinematicChain& kc, const naoth::FSRData& fsr);

private:
    bool supportFoot; // TRUE: left foot; FALSE: right foot;
    bool init;
    Pose3D lastLeftFoot, lastRightFoot, lastHip;
};

#endif //_ODOMETRY_CALCULATOR_H_
