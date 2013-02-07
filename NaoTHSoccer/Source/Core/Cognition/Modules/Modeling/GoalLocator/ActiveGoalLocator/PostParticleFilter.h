/**
* @file PostParticleFilter.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _PostParticleFilter_h_
#define _PostParticleFilter_h_

#include "AGLSampleSet.h"
#include "Representations/Perception/GoalPercept.h"

class PostParticleFilter
{
public:
  class Parameters
  {
  public:
    double motionNoiseDistance;
    double standardDeviationAngle;
    double weightingTreshholdForUpdateWithAngle;
    double processNoiseDistance;
    double resamplingThreshhold;
  };

  void setParams(const Parameters& parameters) { this->parameters = parameters; }

  void updateByOdometry(const Pose2D& odometryDelta);
  void updateByGoalPostPercept(const GoalPercept::GoalPost& post);
  void resampleGT07(bool noise);

  double getConfidenceForObservation(const GoalPercept::GoalPost& post) const;

  // debug stuff
  void drawParticles(const std::string& color, int idx) const;
  void drawSamplesImportance() const;

  AGLSampleSet sampleSet;

private:
  Parameters parameters;
};

#endif // _PostParticleFilter_h_
