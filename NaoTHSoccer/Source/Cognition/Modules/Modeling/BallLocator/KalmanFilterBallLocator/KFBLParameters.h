/**
* @file KFBLParameters.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Declaration of class KFBLParameters copied by MCSLParameters
*/

#ifndef __KFBLParameters_h_
#define __KFBLParameters_h_

#include <Tools/DataStructures/ParameterList.h>

class KFBLParameters: private ParameterList
{
public: 
  KFBLParameters();
  ~KFBLParameters();

  // standart deviations for the gausian angle and distance model

  double sigmaBallPosition;
  double sigmaBallPositionReactive;
  double sigmaBallSpeed;
  double sigmaBallSpeedReactive;

  double processNoise;

  double ballMass;
  double frictionCoefficiant;

  double randomizeFrameSelection;
  
};

#endif //__KFBLParameters_h_
