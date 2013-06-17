/**
* @file AGLSampleSet.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class AGLSampleSet
*/

#ifndef _AGLSampleSet_h_
#define _AGLSampleSet_h_

#include "AGLSample.h"
#include <Tools/Math/Moments2.h>
#include <vector>

class AGLSampleSet
{
private:
  unsigned int numberOfParticles;
  bool isValid;

public: 
  AGLSampleSet(unsigned int n = 10)
    : 
    isValid(false),
    samples(n),
    lastTotalWeighting(1.2) //choosen by observation for usual initial values
  {
  }

  ~AGLSampleSet() {}

  Vector2<double> mean;
  std::vector<AGLSample> samples;

  double lastTotalWeighting; //saves the avarage weighting and decrease it by time in AGL

  /** 
   * sort the particles according to their likelihood
   * with quicksort
   */
  void sort();

  /** 
   * normalize the likelihoods of patricle so thay sum up to 1
   */
  void normalize();

  void setValid();

  void setUnValid();

  bool getIsValid() const;

  /** 
   * reset the likelihoods of patricle to 1/numberOfParticles
   */
  void resetLikelihood();

  /**
   * Access operator.
   * @param index The index of the sample to access.
   */
  AGLSample& operator[](int index) {return samples[index];}

  /**
   * Constant access operator.
   * @param index The index of the sample to access.
   */
  const AGLSample& operator[](int index) const {return samples[index];}

  long unsigned int size() const { return samples.size(); }

  AGLSample meanOfLargestCluster(Moments2<2>& moments);

  void drawCluster(unsigned int clusterId);

  

private:
  void quicksort(int low, int high);
};

#endif //__AGLSampleSet_h_
