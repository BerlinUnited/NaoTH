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

  // standart deviations for the gausian angle and distance model

  double sigmaDistanceOwnPose;
  double sigmaAngleOwnPose;

  double sigmaDistanceGoalModel;
  double sigmaAngleGoalModel;

  double sigmaDistanceGoalPost;
  double sigmaAngleGoalPost;

  double sigmaDistanceCenterCircle;
  double sigmaAngleCenterCircle;

  double sigmaDistanceCorner;
  double sigmaAngleCorner;

  double sigmaDistanceLine;
  double sigmaAngleLine;

  double thresholdCanopy;

  double resamplingThreshhold;

  double processNoiseDistance;
  double processNoiseAngle;

  double motionNoiseDistance;
  double motionNoiseAngle;

  // updates
  double updateByGoals;
  double updateByOldPose;
  double updateByLinesTable;
  double updateByCornerTable;
  double updateByCenterCircle;
  double updateByFlags;

  virtual void print(std::ostream& stream) const {}

};

#endif //__PFBLParameters_h_
