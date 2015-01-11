/*
* @file Moments2.h
*
* @author <a href="mailto:thermann@informatik.hu-berlin.de">Tobias Hermann</a>
* Implementation of class Moments2
*/

#ifndef __Moments2_H_
#define __Moments2_H_

#include "Tools/Math/Vector2.h"

template<int MAX_ORDER>
class Moments2
{
public:
  Moments2()
  {
    reset();
  };

  // raw moments Mpq of order (p+q) are defined as
  // Mpq = sum(sum(x^p * y^q * f(x, y)))
  double rawMoment[((MAX_ORDER+1)*(MAX_ORDER+2))/2]; /**< The raw Moments. */

  //add another point to the distribution
  //whos moments to keep track of
  void add(const Vector2<int>& point, double weight)
  {
    for(int order=0; order<=MAX_ORDER; order++) //go through all orders
      for(int p=0; p<=order; p++)             //for every order go through all moments of that order
        rawMoment[((order*(order+1))/2)+p] += pow((double)point.x, (double)p)*pow((double)point.y, (double)(order-p))*weight; //update the corresponding raw moment M(p, order-p)
  }//end add

  //if no weight for the point to add is specified, use weight 1
  void add(const Vector2<int>& point)
  {
    add(point, 1.0);
  }//end add

  //return the raw moment Mpq
  double getRawMoment(int p, int q) const
  {
    if(p+q > MAX_ORDER) return 0;

    return rawMoment[(((p+q)*(p+q+1))/2)+p];
  }//end getRawMoment

  //return the centroid of the distribution so far
  //the centroid (cx, cy) is defined as
  //(cx, cy) = (M10/M00, M01/M00)
  //if no points were added to the distribution yet
  //(i.e. M00=0) return (0, 0)
  Vector2<double> getCentroid() const
  {
    Vector2<double> centroid(0.0, 0.0);
    if(getRawMoment(0, 0)!=0)
    {
      centroid.x = getRawMoment(1, 0)/getRawMoment(0, 0);
      centroid.y = getRawMoment(0, 1)/getRawMoment(0, 0);
    }//end if

    return centroid;
  }//end getCentroid

  //reset all raw moments to 0
  void reset()
  {
    for(int n=0; n<((MAX_ORDER+1)*(MAX_ORDER+2))/2; n++)
      rawMoment[n] = 0;
  }//end clear


  //return the central moment upq
  //the central moment upq of order p+q is defined as
  //sum(sum( (x-cx)^p * (y-cy)^q * f(x, y) ))
  //where (cx, cy) is the centroid of the distribution
  double getCentralMoment(int p, int q) const
  {
    Vector2<double> c = getCentroid();
    double moment = 0;

    for(int m=0; m<=p; m++)
      for(int n=0; n<=q; n++)
        moment += binomialCoefficient(p, m)*binomialCoefficient(q, n)*pow(-c.x, p-m)*pow(-c.y, q-n)*getRawMoment(m, n);

    return moment;
  }//end getCentralMoment

  void getAxes(Vector2<double>& major, Vector2<double>& minor) const
  {
    double u20 = getCentralMoment(2, 0);
    double u02 = getCentralMoment(0, 2);
    double u11 = getCentralMoment(1, 1);
    double u00 = getRawMoment(0, 0);

    //if no pixel was added to the distribution
    //no axes can be calculated
    if(u00==0) return;

    //the covariance Matrix of the pixel distribution is defined as
    // u20/u00   u11/u00
    // u11/u00   u02/u00

    //calculate the elements of this matrix
    double us20=u20/u00;
    double us02=u02/u00;
    double us11=u11/u00;

    //determine the larger eigenvalue of this matrix
    double l1=(us20+us02)/2 + sqrt(4*us11*us11+(us20-us02)*(us20-us02))/2;
    //determine the smaller eigenvalue of this matrix
    double l2=(us20+us02)/2 - sqrt(4*us11*us11+(us20-us02)*(us20-us02))/2;

    //calculate the angle theta of the unit eigenvector associated 
    //with the largest eigenvalue
    double theta = 0.5*atan2(2 * us11, us20 - us02);

    //calculate the unit vector with angle theta
    major.x=cos(theta);
    major.y=sin(theta);

    //calculate the unit vector with angle theta+pi/2
    minor.x= -major.y;
    minor.y=  major.x;

    //scale the axes with the square root of their corresponding eigenvalues
    //i.e. the standard deviation along those axes
    major*=sqrt(l1);
    minor*=sqrt(l2);

  }//end getAxes

  //returns the binomial coefficient (n over k)
  int binomialCoefficient(int n, int k) const
  {
    return (factorial(n)/(factorial(k)*factorial(n-k)));
  }//end binomialCoefficient


  //returns the factorial of n
  int factorial(int n) const
  {
    int tmp=1;
    for(int c=1; c<=n; c++)
      tmp*=c;

    return tmp;
  }//end factorial


};//end class Moments2


#endif // __Moments2_H_
