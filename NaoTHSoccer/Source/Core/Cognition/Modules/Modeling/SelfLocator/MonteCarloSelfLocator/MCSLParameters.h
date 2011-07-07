/**
* @file MCSLParameters.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class MCSLParameters
*/

#ifndef __MCSLParameters_h_
#define __MCSLParameters_h_

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/DataStructures/Printable.h>

class MCSLParameters: public ParameterList, public naoth::Printable
{
public: 
  MCSLParameters();
  ~MCSLParameters();

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

#endif //__MCSLParameters_h_
