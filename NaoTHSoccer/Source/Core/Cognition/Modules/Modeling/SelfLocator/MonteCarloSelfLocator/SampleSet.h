/**
* @file SampleSet.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class SampleSet
*/

#ifndef __SampleSet_h_
#define __SampleSet_h_

#include "Sample.h"
#include <Tools/Math/Moments2.h>
#include <vector>

class SampleSet
{
public:
  const unsigned int& numberOfParticles; /** TODO: make it as parameter */

private:
  int _numberOfParticles;

public: 
  SampleSet(int n = 100) 
    : 
    numberOfParticles(_numberOfParticles),
    _numberOfParticles(n),
    samples(n)
  {
  }

  ~SampleSet() {}

  std::vector<Sample> samples;

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
  Sample& operator[](int index) {return samples[index];}

  unsigned int size() const { return numberOfParticles; }

  /**
   * Constant access operator.
   * @param index The index of the sample to access.
   */
  const Sample& operator[](int index) const {return samples[index];}


  Sample meanOfLargestCluster(Moments2<2>& moments);

  void drawCluster(unsigned int clusterId);

private:
  void quicksort(int low, int high);
};

#endif //__SampleSet_h_
