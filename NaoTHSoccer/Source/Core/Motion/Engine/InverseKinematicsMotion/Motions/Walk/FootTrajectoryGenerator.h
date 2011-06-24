/**
 * @File:   FootTrajectoryGenerator.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * foot trajectory for walking
 */

#ifndef FOOTTRAJECTORYGENERATOR_H
#define	FOOTTRAJECTORYGENERATOR_H

#include "Tools/Math/Pose3D.h"
#include "Tools/Math/CubicSpline.h"

class FootTrajectorGenerator
{
public:
  static Pose3D genTrajectory(const Pose3D& oldFoot, const Pose3D& targetFoot,
    double cycle, double samplesDoubleSupport, double samplesSingleSupport, double extendDoubleSupport,
    double stepHeight, double footPitchOffset, double footYawOffset, double footRollOffset, double curveFactor);
};


#endif	/* FOOTTRAJECTORYGENERATOR_H */

