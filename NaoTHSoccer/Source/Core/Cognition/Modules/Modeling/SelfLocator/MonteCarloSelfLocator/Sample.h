/**
* @file Sample.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Sample
*/

#ifndef __Sample_h_
#define __Sample_h_

#include "Tools/Math/Pose2D.h"


class Sample2D
{
public: 
  Sample2D() : cluster(0) {}

  unsigned int cluster; /** id of the cluster this sample belongs to */

  virtual const Vector2<double>& getPos() const = 0;
};


class Sample: public Pose2D, public Sample2D
{
public: 
  Sample() : likelihood(0.0){}
  ~Sample() {}

  double likelihood;

  virtual const Vector2<double>& getPos() const
  {
    return translation;
  }
};

#endif //__Sample_h_
