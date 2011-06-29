/**
* @file AGLParameters.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Declaration of class AGLParameters
*/

#ifndef __AGLLParameters_h_
#define __AGLLParameters_h_

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/DataStructures/Printable.h>

class AGLParameters: public ParameterList, public naoth::Printable
{
public: 
  AGLParameters();
  ~AGLParameters();

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

#endif //__AGLParameters_h_
