/**
* @file SampleSet.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class SampleSet
*/

#ifndef _SampleSet_h_
#define _SampleSet_h_

#include "Sample.h"
#include <Tools/Math/Moments2.h>
#include <vector>

class SampleSet
{

public:
  SampleSet(unsigned int n = 100)
    :
    samples(n),
    numberOfParticles(n)
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
  void normalize(double offset = 0.0);

  /** 
   * reset the likelihoods of patricle to 1/numberOfParticles
   */
  void resetLikelihood();

  /**
   * Access operator.
   * @param index The index of the sample to access.
   */
  inline Sample& operator[](int index) {return samples[index];}

  inline unsigned int size() const { return numberOfParticles; }

  /**
   * Constant access operator.
   * @param index The index of the sample to access.
   */
  inline const Sample& operator[](int index) const {return samples[index];}

  const Sample& SampleSet::getMostLikelySample() const;
  Sample meanOfLargestCluster(Moments2<2>& moments) const;

  // TODO: move it out of here
  void drawCluster(unsigned int clusterId) const;
  void drawImportance(bool arrows = true) const;

private:
  void quicksort(int low, int high);

  unsigned int numberOfParticles;
};

#endif //_SampleSet_h_
