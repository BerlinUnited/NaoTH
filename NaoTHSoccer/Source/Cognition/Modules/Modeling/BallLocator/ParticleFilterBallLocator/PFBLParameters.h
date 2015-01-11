/**
* @file PFBLParameters.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class PFBLParameters
*/

#ifndef __PFBLParameters_h_
#define __PFBLParameters_h_

#include <Tools/DataStructures/ParameterList.h>

class PFBLParameters: public ParameterList
{
public: 
  PFBLParameters();
  ~PFBLParameters();

  double frictionCoefficiant;
  double odometryNoiseFactor;
  double velocityNoiseFactor;

  double sigmaAngle;
  double sigmaDistance;

  double speedWehnParticleIsMoving;
};

#endif //__PFBLParameters_h_
