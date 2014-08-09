/**
* @file MCSLTools.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class MCSLTools
*/

#ifndef _MCSLTools_h_
#define _MCSLTools_h_

#include "SampleSet.h"
#include "Tools/Math/Common.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Math/Probabilistics.h"

namespace mcsl
{

inline void applySimpleNoise(Sample& sample, double distanceNoise, double angleNoise)
{
  sample.translation.x += (Math::random()-0.5)*distanceNoise;
  sample.translation.y += (Math::random()-0.5)*distanceNoise;
  sample.rotation = Math::normalize(sample.rotation + (Math::random()-0.5)*angleNoise);
}

// ACHTUNG: not tested yet(!)
inline void applyDirectionalNoise(Sample& sample, double xNoise, double yNoise, double angleNoise)
{
  sample.translate((Math::random()-0.5)*xNoise, (Math::random()-0.5)*yNoise);
  sample.rotation = Math::normalize(sample.rotation + (Math::random()-0.5)*angleNoise);
}

inline void createRandomSample(const Geometry::Rect2d& rect, Sample& sample)
{
  sample.translation.x = Math::random(rect.min().x, rect.max().x);
  sample.translation.y = Math::random(rect.min().y, rect.max().y);
  sample.rotation = Math::random(-Math::pi, Math::pi);
}

inline void initializeSampleSet(const Geometry::Rect2d& rect, SampleSet& sampleSet)
{
  double likelihood = 1.0/static_cast<double>(sampleSet.size());
  for (size_t i = 0; i < sampleSet.size(); i++)
  {
    Sample& sample = sampleSet[i];
    createRandomSample(rect, sample);
    sample.likelihood = likelihood;
  }
}

inline void initializeSampleSetFixedRotation(const Geometry::Rect2d& rect, double angle, SampleSet& sampleSet)
{
  double likelihood = 1.0/static_cast<double>(sampleSet.size());
  for (size_t i = 0; i < sampleSet.size(); i++)
  {
    Sample& sample = sampleSet[i];
    sample.translation.x = Math::random(rect.min().x, rect.max().x);
    sample.translation.y = Math::random(rect.min().y, rect.max().y);
    sample.rotation = angle;
    sample.likelihood = likelihood;
  }
}

inline double computeAngleWeight( double measured, double expected, double standardDeviation)
{ 
  double diff = Math::normalize(expected - measured);
  //return Math::gaussianProbability(diff, standardDeviation);
  return std::max(exp(-0.5 * Math::sqr(diff / standardDeviation)), 1e-6);
}


inline double computeDistanceWeight( double measured, double expected, double cameraHeight, double standardDeviation)
{
  // estimate the camera coordinates
  const double measuredDistanceAsAngle = atan2(measured, cameraHeight);
  const double expectedDistanceAsAngle = atan2(expected, cameraHeight);

  double diff = Math::normalize(expectedDistanceAsAngle - measuredDistanceAsAngle);
  //double result = Math::gaussianProbability(diff, standardDeviation);
  return std::max(exp(-0.5 * Math::sqr(diff / standardDeviation)), 1e-6);
}

}

#endif //__MCSLTools_h_
