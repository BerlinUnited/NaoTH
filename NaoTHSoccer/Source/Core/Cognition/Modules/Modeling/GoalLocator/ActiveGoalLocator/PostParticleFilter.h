/**
* @file PostParticleFilter.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _PostParticleFilter_h_
#define _PostParticleFilter_h_

#include "AGLSampleSet.h"
#include "Representations/Perception/GoalPercept.h"

//Tools
#include <vector>

class PostParticleFilter
{
public:
  class Parameters
  {
  public:
    // parameters for the motion model
    double motionNoiseDistance;
    double motionNoiseRotation;
    // parameters for the observation model
    double standardDeviationAngle;
    double standardDeviationDistance;
    // parameters for the filter resampling
    double processNoiseDistance;
    double resamplingThreshhold;
    double particlesToReset;
  };

  void setParams(const Parameters& parameters) { this->parameters = parameters; }

  void updateByOdometry(const Pose2D& odometryDelta);
  void updateByGoalPostPercept(const GoalPercept::GoalPost& post, const double cameraHeight);
  void resampleGT07(std::vector<GoalPercept::GoalPost> postArr, bool noise);

  double getConfidenceForObservation(const GoalPercept::GoalPost& post, const double cameraHeight) const;

  double getWeightingByPercept(const AGLSample& sample, const GoalPercept::GoalPost& post, const double cameraHeight) const;

  // debug stuff
  void drawParticles(const std::string& color, int idx) const;
  void drawSamplesImportance() const;

  AGLSampleSet sampleSet;

  RingBuffer<Vector2<double>, 10> PerceptBuffer;

private:
  Parameters parameters;
};

#endif // _PostParticleFilter_h_
