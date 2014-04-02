/**
* @file ProbabilisticQuadCompas.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class ProbabilisticQuadCompas
*/

#ifndef _ProbabilisticQuadCompas_h_
#define _ProbabilisticQuadCompas_h_

#include <Tools/Math/Common.h>
#include <vector>

class ProbabilisticQuadCompas
{
private:
  const double normal_value;
  std::vector<double> angles;
  const double bin_size;
  double smoothing;
  bool normalized;

public:
  ProbabilisticQuadCompas(double smoothing = 0.4, int bins = 18) 
    : 
    normal_value(1.0/(double)bins),
    angles(bins, 0.0),
    bin_size(Math::pi_2/bins), // size of a bin in rad
    smoothing(smoothing),
    normalized(false)
  {
    assert(bins > 0);
  }

  inline void add(double angle, double wight = 1.0) {
    for(size_t i = 0; i < angles.size(); i++) {
      double x = i*bin_size;
      angles[i] += exp(-Math::sqr(sin(2.0*(angle - x)) / smoothing))*wight;
    }
    normalized = false;
  }

  inline void normalize() {
    double sum = 0.0;
    for(size_t i = 0; i < angles.size(); i++) {
      sum += angles[i];
    }

    if(sum != 0) {
      for(size_t i = 0; i < angles.size(); i++) {
        angles[i] /= sum;
      }
    } else {
      double n = 1.0/angles.size();
      for(size_t i = 0; i < angles.size(); i++) {
        angles[i] = n;
      }
    }
    normalized = true;
  }

  inline void reset() {
    normalized = false;
    std::fill(angles.begin(), angles.end(), 0.0);
  }

  inline double probability(double angle) const {
    assert(normalized);
    double a = Math::normalize(angle) + Math::pi2;
    return (*this)[(int)(a / bin_size + 0.5)];
  }

  inline double operator[] (int i) const {
    assert(normalized);
    return angles[i % angles.size()];
  }

  inline size_t size() { return angles.size(); }

  inline void setSmoothing(double s) { smoothing = s; }

  inline bool isValid() const { return normalized; }
};

#endif // _ProbabilisticQuadCompas_h_
