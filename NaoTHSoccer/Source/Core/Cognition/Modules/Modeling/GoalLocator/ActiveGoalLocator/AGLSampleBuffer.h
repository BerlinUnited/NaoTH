/**
* @file AGLSampleSet.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class AGLSampleSet
*/

#ifndef __AGLSampleBuffer_h_
#define __AGLSampleBuffer_h_

//#include "AGLSample.h"
#include <Tools/Math/Moments2.h>
#include <vector>

#include "Representations/Perception/GoalPercept.h"
#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/Sample.h"
#include "Tools/ColorClasses.h"

class AGLBSample: public Sample2D
{
public:
  AGLBSample() 
  : 
    likelihood(0.0),
    frameNumber(0)
  {}
  ~AGLBSample() {}

  double likelihood;
  Vector2<double> translation;
  ColorClasses::Color color;
  unsigned int frameNumber;

  virtual const Vector2<double>& getPos() const
  {
    return translation;
  }

};

class AGLSampleBuffer
{
public:
  const unsigned int& numberOfParticles; /** TODO: make it as parameter */

private:
  unsigned int _numberOfParticles;

public: 
  AGLSampleBuffer(unsigned int n = 100)
    :
    numberOfParticles(_numberOfParticles),
    _numberOfParticles(n)
    //samples(n)
  {
  }

  ~AGLSampleBuffer() {}

  //std::vector<AGLSample> samples;
  RingBuffer<AGLBSample, 67> samples;

  /** 
   * sort the particles according to their likelihood
   * with quicksort
   */
  void sort();

  /** 
   * normalize the likelihoods of patricle so thay sum up to 1
   */
  void normalize();

  /** 
   * reset the likelihoods of patricle to 1/numberOfParticles
   */
  void resetLikelihood();

  /**
   * Access operator.
   * @param index The index of the sample to access.
   */
  AGLBSample& operator[](int index) {return samples.getEntry(index);}

  /**
   * Constant access operator.
   * @param index The index of the sample to access.
   */
  const AGLBSample& operator[](int index) const {return samples.getEntry(index);}

  unsigned int size() const
  {
    return samples.getNumberOfEntries();
  }

  AGLSampleBuffer meanOfLargestCluster(Moments2<2>& moments);

  void drawCluster(unsigned int clusterId);

private:
  void quicksort(int low, int high);
};

#endif //__AGLSampleBuffer_h_
