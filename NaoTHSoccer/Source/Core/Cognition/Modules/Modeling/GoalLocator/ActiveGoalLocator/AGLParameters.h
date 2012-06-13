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

  double standardDeviationDist;
  double standardDeviationAngle;

  double timeFilterRange;

  double sigmaWeightingThreshhold;
  double thresholdCanopy;
  double motionNoiseDistance;

  double weightingTreshholdForUpdateWithAngle;
  double processNoiseDistance;
  double resamplingThreshhold;

  double possibleGoalWidhtError;

  double deletePFbyTotalWeightingThreshold;

  virtual void print(std::ostream& stream) const {}

};

#endif //__AGLParameters_h_
