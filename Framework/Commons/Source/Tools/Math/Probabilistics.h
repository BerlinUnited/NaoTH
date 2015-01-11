/**
 * @file Math/Probabilistics.h
 *
 * This contains some probabilistic functions
 *
 * @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
 */


#ifndef __Probabilistics_h__
#define __Probabilistics_h__

#include "Common.h"
#include <algorithm>

namespace Math {

/** constant for triangular distribution*/
const double sqrt6dividedBy2 = sqrt(6.0)/2.0;

/**
* Returns a gaussian random deviate
* @return As mentioned above
*/
inline double randomGauss()
{
  double r,v1,v2;
  do 
  {
    v1=2.0f*random()-1.0;
    v2=2.0f*random()-1.0;
    r=v1*v1+v2*v2;
  } while (r >= 1.0 || r==0);
  const double fac(sqrt(-2.0*log(r)/r));
  return v1*fac;
}

/**
* Returns the probability of a value in a gaussian distribution
* @param x The value
* @param s The standard deviation
* @return The probability density at x
*/
inline double gaussianProbability(double x, double s)
{
  return std::max(1.0 / (s * sqrt2pi) * exp(-0.5 * sqr(x / s)), 1e-6);
}

/**
* Sampling from approximated normal distribution with zero mean and 
* standard deviation b. C.f. "Probabilistic Robotics", Table 5.4
* @param b The standard deviation
* @return The sampled value
*/
inline double sampleNormalDistribution(double b)
{
  double result(0.0);
  for(int i=0; i< 12; i++)
    result += 2.0*((Math::random() - 0.5)*b);
  return result/2.0;
}

/**
* Sampling from approximated normal distribution with zero mean and 
* standard deviation b. C.f. "Probabilistic Robotics", Table 5.4
* This is an integer version which uses randomFast, so use it only for large n
* @param b The standard deviation
* @return The sampled value
*/
inline int sampleNormalDistribution(int b)
{
  if(b != 0)
  {
    int result(0);
    for(int i=0; i< 12; i++)
      result += random(2*b) - b;
    return result/2;
  }
  else
    return 0;
}

/**
* Sampling from a triangular distribution with zero mean and 
* standard deviation b. C.f. "Probabilistic Robotics", Table 5.4
* @param b The standard deviation
* @return The sampled value
*/
inline double sampleTriangularDistribution(double b)
{
  double randResult = 2.0*((random() - 0.5)*b) + 2.0*((random() - 0.5)*b);
  return sqrt6dividedBy2 * randResult;
}

/**
* Sampling from a triangular distribution with zero mean and 
* standard deviation b. C.f. "Probabilistic Robotics", Table 5.4
* This is an integer version which uses randomFast, so use it only for large n
* @param b The standard deviation
* @return The sampled value
*/
inline int sampleTriangularDistribution(int b)
{
  if(b!=0)
  {
    int randResult = (random(2*b) - b) + (random(2*b) - b);
    return static_cast<int>(sqrt6dividedBy2 * randResult);
  }
  else
    return 0;
}

}//end namespace Math

#endif // __Probabilistics_h__
