/**
* @file PFBLParameters.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class PFBLParameters
*/

#ifndef __PFBLParameters_h_
#define __PFBLParameters_h_

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/DataStructures/Printable.h>

class PFBLParameters: public ParameterList, public naoth::Printable
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

  virtual void print(std::ostream& stream) const {}

};

#endif //__PFBLParameters_h_
