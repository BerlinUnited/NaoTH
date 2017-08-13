#ifndef __Math_Common_h__
#define __Math_Common_h__

#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <assert.h>
#include <cmath>

#include <algorithm>
#include <vector>

#ifdef WIN32 /* needed for _isnan */
#include <float.h>
#endif

namespace Math {

  /** constant for a half circle*/
  const double pi = 3.1415926535897932384626433832795;
  /** constant for a full circle*/
  const double pi2 = 2.0*3.1415926535897932384626433832795;
  /** constant for three quater circle*/
  const double pi3_2 = 1.5*3.1415926535897932384626433832795;
  /** constant for a quarter circle*/
  const double pi_2 = 0.5*3.1415926535897932384626433832795;
  /** constant for a 1 degree*/
  const double pi_180 = 3.1415926535897932384626433832795/180.0;
  /** constant for a 1/8 circle*/
  const double pi_4 = 3.1415926535897932384626433832795*0.25;
  /** constant for a 3/4 circle*/
  const double pi3_4 = 3.1415926535897932384626433832795*0.75;
  /** constant for an expression used by the gaussian function*/
  const double sqrt2pi = sqrt(2.0*pi);
  /** constant for e*/
  const double e = 2.71828182845902353602874713527;
  /** the gravity of degree of latitude 45 */
  const double g = 9.80620;


  /**
  * defines the isNan function for linux and windows
  */
  template <class T>
  inline bool isNan(T f)
  {
#ifdef WIN32
    return _isnan(static_cast<double> (f)) != 0;
#else
    return std::isnan(static_cast<float> (f));
#endif
  }

  /**
  * defines the isInf function for linux and windows
  */
  inline bool isInf(double x)
  {
#ifdef WIN32
    return !_finite(x);
#else
    return std::isinf(x);
#endif
  }//end isInf

  /**
  * defines the sign of a
  */
  #ifdef sgn
  #undef sgn
  #endif

  template <class T>
  inline int sgn(const T& a) {
    return ( (a) < 0 ? -1 : ((a)==0) ? 0 : 1 );
  }  

  /**
  * defines the square of a value
  */
  #ifdef sqr
  #undef sqr
  #endif

  template <class T>
  inline T sqr(const T& a)
  {
    return a*a;
  }

  /**
  * mathematical rounding for floating point types
  */
  template <class T>
  inline T round(const T& a)
  {
    return floor(a + static_cast<T>(0.5));
  }

  inline double sec(const double a){return 1.0/cos(a);}
  inline double cosec(const double a){return 1.0/sin(a);}

  /** 
   * Converts angle from rad to degrees.
   * @param angle code in rad
   * @return angle coded in degrees
   */
  inline double toDegrees(double angle){return angle / pi_180;}

  /** 
   * Converts angle from degrees to rad.
   * @param degrees angle coded in degrees
   * @return angle coded in rad
   */
  inline double fromDegrees(double degrees){return degrees * pi_180;}

  /** 
  * reduce angle to [-pi..+pi[
  * @param data angle coded in rad
  * @return normalized angle coded in rad
  */
  inline double normalize(double data)
  {
    if (data < pi && data >= -pi) return data;
    double ndata = data - ((int)(data / pi2))*pi2;
    if (ndata >= pi) {
      ndata -= pi2; 
    } else if (ndata < -pi) {
      ndata += pi2;
    }
    return ndata;
  }

  /**
  * reduce angle to [0..+pi[
  * @param data angle coded in rad
  * @return normalized angle coded in rad
  */
  inline double normalizePositive(double data)
  {
    double ndata = normalize(data);
    if (ndata < 0) {
      ndata += pi;
    }
    return ndata;
  }

  /** 
   * This method normalizes an angle. This means that the resulting
   * angle lies between (-180, 180] degrees.
   * @param[in] angle the angle which must be normalized
   * @return the result of normalizing the given angle
   * TODO: this function takes a lot of time when the angle is very big (say angle=1e+61)
   */
  template<class TYPE>
  inline TYPE normalizeAngle(TYPE angle)
  {
      while (angle > pi) angle -= pi2;
      while (angle <= -pi) angle += pi2;

      return ( angle);
  }


  /**
  * The function returns a random number in the range of [0..1].
  * @return The random number.
  */
  inline double random() {return double(rand()) / RAND_MAX;}

  /**
  * The function returns a random integer number in the range of [0..n-1].
  * @param n the number of possible return values (0 ... n-1)
  * @return The random number.
  */
  inline int random(int n) { assert(n > 0); return ((int)(random()*n))%n; }


  /** 
   * generate a random int value in [min, max]
   * @param[in] min the min value of the random number
   * @param[in] max the max value of the random number
   * @return the random number
   */
  inline int random(int min, int max)
  {
    return min + random(max - min + 1);
  }

  // TODO: is not working properly for int
  /** 
   * generate a random value in (min,max) 
   * @param[in] min the min value of the random number
   * @param[in] max the max value of the random number
   * @return the random number
   */
  template < class T >
  inline T random(T min, T max)
  {
      static bool haveSet = false;
      if (!haveSet)
      {
          srand((unsigned int)time(0));
          haveSet = true;
      }//end if
      return T(random()*((double)(max - min))) + min;
  }//end random

  /** 
   * set the value to range [min,max]
   * @param[in] x the orginal value
   * @param[in] min the range left
   * @param[in] max the range right
   * @return the value be clamped
   */
  template < class T >
  inline T clamp(T x, T min, T max)
  {
    if (x > max) return max;
    if (x < min) return min;
    return x;
  }

  /**
  * draw number from normal random distribution
  * @param mean the mean value
  * @param std the standard deviation
  * @return the random number
  */
  inline double normal(const double &mean, const double &std)
  {
    static const double r_max=RAND_MAX+1.0;
    return std*sqrt(-2.0*log((rand()+1.0)/r_max))*sin(2.0*pi*rand()/r_max)+mean;
  }

  /**
  * http://en.wikipedia.org/wiki/Mean_of_circular_quantities
  */
  inline double meanAngle(double a, double b) {
    return atan2( sin(a)+sin(b), cos(a)+cos(b) );
  }

  /* TODO: This needs to be tested
  template<typename T>
  T vectorMeanAngle(const std::vector<T> values) {
    // TODO: remove assert when the method was tested 
    assert(true);
    
    double a = 0;
    double b = 0;
    for (size_t i = 0; i < values.size(); ++i)
    {
      a += sin(values[i]);
      b += cos(values[i]);
    }
    return atan2(a, b);
  }
  */

  // NOTE: the input vector is copied and sorted internally, this can be slow for large vectors
  template<typename T>
  T medianMean(std::vector<T> values) {
    if(values.empty()){ return 0; }

    size_t size = values.size();
    std::sort(values.begin(), values.end());

    if (size  % 2 == 0) {
        return (values[size / 2 - 1] + values[size / 2]) / 2;
    } else {
        return values[size / 2];
    }
  }

  // NOTE: the input vector is copied and sorted internally, this can be slow for large vectors
  template<typename T>
  T medianMax(std::vector<T> values) {
    if (values.empty()){ return 0; }

    size_t size = values.size();
    std::sort(values.begin(), values.end());

    if (size % 2 == 0) {
      return (std::max(values[size / 2 - 1], values[size / 2]));
    }
    else {
      return values[size / 2];
    }
  }

  // NOTE: the input vector is copied and sorted internally, this can be slow for large vectors
  template<typename T>
  T median(std::vector<T> values) {
    if(values.empty()){ return 0; }
    std::nth_element(values.begin(), values.begin() + values.size()/2, values.end());
    return values[values.size()/2];
  }

  // NOTE: the input vector is copied and sorted internally, this can be slow for large vectors
  //Just a wrapper for nth_element -> median is index = values.size()/2
  template<typename T>
  T max(const std::vector<T>& values) {
    return *std::max_element(values.begin(), values.end());
  }

}//end namespace Math

#endif // __Math_Common_h__
