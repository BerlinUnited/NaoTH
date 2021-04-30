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
#include "Tools/Debug/DebugDrawings.h"

class SampleSet
{

public:
  typedef size_t size_type;

  SampleSet(size_t n = 100)
    :
    samples(n)
  {
  }

  ~SampleSet() {}

  /** 
  * sort the particles according to their likelihood
  * with quicksort
  */
  void sort(bool descending = true);

  /** 
  * normalize the likelihoods of patricle so thay sum up to 1
  * offset - a value added to all likelihoods to prevent them
  *          to become too small. The offset should be typically 
  *          a value in [0,1]
  *
  * E.g., the call normalize(0.1) is equivalent to 
      
	  // normalize 
	  normalize();
	  
	  // add the relative offset
	  for(size_t i = 0; i < samples.size(); ++i) {
		samples[i].likelihood += 0.1;
	  }
	
	  // normalize again
	  normalize();
     
  */
  void normalize(double offset = 0.0);

  /** 
  * reset the likelihoods of patricle to 1/numberOfParticles
  */
  void resetLikelihood();

  /** 
  * set the likelihood to th given value for every particle
  */
  void setLikelihood(double v);

  /**
  * Access operator.
  * @param index The index of the sample to access.
  */
  inline Sample& operator[](int index) {return samples[index];}
  inline Sample& operator[](size_t index) {return samples[index];}

  /**
  * yeah, guess what it does ...
  */
  inline size_t size() const { return samples.size(); }

  /**
   * Constant access operator.
   * @param index The index of the sample to access.
   */
  inline const Sample& operator[](int index) const {return samples[index];}

  const Sample& getMostLikelySample() const;
  Sample meanOfLargestCluster(Moments2<2>& moments) const;
  Sample meanOfCluster(Moments2<2>& moments, int idx) const;

  // TODO: move it out of here
  void drawCluster(DrawingCanvas2D& canvas, unsigned int clusterId) const;
  void drawImportance(DrawingCanvas2D& canvas, bool arrows = true) const;

private:
  std::vector<Sample> samples;

  void quicksort(int d, int low, int high);
};

#endif //_SampleSet_h_
