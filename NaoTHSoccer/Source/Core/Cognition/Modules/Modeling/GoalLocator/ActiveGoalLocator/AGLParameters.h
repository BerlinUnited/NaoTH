/**
* @file AGLParameters.h
*
* @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
* Declaration of class AGLParameters
*/

#ifndef _AGLLParameters_h_
#define _AGLLParameters_h_

#include <Tools/DataStructures/ParameterList.h>
#include <Tools/DataStructures/Printable.h>

#include "PostParticleFilter.h"

class AGLParameters: public ParameterList, public naoth::Printable
{
public: 
  AGLParameters();
  ~AGLParameters();

  //double standardDeviationDist;
  //double standardDeviationAngle;
  //double processNoiseDistance;
  //double motionNoiseDistance;
  //double resamplingThreshhold;
  PostParticleFilter::Parameters particleFilter;


  double weightingTreshholdForUpdateWithAngle;
  double timeFilterRange;

  //double sigmaWeightingThreshhold;
  double thresholdCanopy;

  double possibleGoalWidhtError;

  double deletePFbyTotalWeightingThreshold;

  virtual void print(std::ostream& stream) const {}

};

#endif //_AGLParameters_h_
