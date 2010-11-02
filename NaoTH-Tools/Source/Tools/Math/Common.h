#ifndef __Math_Common_h__
#define __Math_Common_h__

#include <math.h>
#include <stdlib.h>
#include <ctime>


namespace Math {

    template <class T>
    bool isNan(T x)
    {
      return x != x;  
    }

    /**
    * defines the sign of a
    */
    #ifdef sgn
    #undef sgn
    #endif

    template <class T>
    int sgn(const T& a)
    {
      return ( (a) < 0 ? -1 : ((a)==0) ? 0 : 1 );
    }  

    /**
    * defines the square of a value
    */
    #ifdef sqr
    #undef sqr
    #endif

    template <class T>
    T sqr(const T& a)
    {
      return a*a;
    }

    /**
    * mathematical rounding for floating point types
    */
    template <class T>
    T round(const T& a)
    {
      return floor(a + static_cast<T>(0.5));
    }

    inline double sec(const double a){return 1.0/cos(a);}
    inline double cosec(const double a){return 1.0/sin(a);}

    /** @name constants for some often used angles */
    ///@{
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
    /** constant for a 3/8 circle*/
    const double pi3_4 = 3.1415926535897932384626433832795*0.75;
    ///@}

    /** @name constant for bayesian filter */
    ///@{
    /** constant for e*/
    const double e = 2.71828182845902353602874713527f;
    ///@}

    /** 
     * Converts angle from rad to degrees.
     * \param angle code in rad
     * \return angle coded in degrees
     */
    inline double toDegrees(double angle){return angle * 180.0 / pi;}

    /** Converts angle from degrees to rad.
     * \param degrees angle coded in degrees
     * \return angle coded in rad
     */
    inline double fromDegrees(double degrees){return degrees * pi_180;}

    /** 
    * reduce angle to [-pi..+pi[
    * \param data angle coded in rad
    * \return normalized angle coded in rad
    */
    inline double normalize(double data)
    {
      if (data < pi && data >= -pi) return data;
      double ndata = data - ((int)(data / pi2))*pi2;
      if (ndata >= pi)
      {
        ndata -= pi2; 
      }
      else if (ndata < -pi)
      {
        ndata += pi2;
      }
      return ndata;
    }//end normalize

    /** This method normalizes an angle. This means that the resulting
     *  angle lies between -180 and 180 degrees.
     *  @param[in] angle the angle which must be normalized
     *  @return the result of normalizing the given angle
     * TODO: this function takes a lot of time when the angle is very big (say angle=1e+61)
     */
    template<class TYPE>
    inline TYPE normalizeAngle(TYPE angle)
    {
        while (angle > pi) angle -= pi2;
        while (angle < -pi) angle += pi2;

        return ( angle);
    }//end normalizeAngle


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
    inline int random(int n) {return (int)(random()*n*0.999999);}

    // TODO: is not working properly for int
    /** generate a random value in (min,max) 
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

    /** set the value to range [min,max]
     *  @param[in] x the orginal value
     *  @param[in] min the range left
     *  @param[in] max the range right
     *  @return the value be clamped
     */
    template < class T >
    T clamp(T x, T min, T max)
    {
        if (x > max) return max;
        if (x < min) return min;
        return x;
    }//end clamp

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
    }//end normal

}//end namespace Math

#endif // __Math_Common_h__
