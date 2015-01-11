/**
* @file Sample.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Sample
*/

#ifndef _Sample_h_
#define _Sample_h_

#include "Tools/Math/Pose2D.h"


class Sample2D
{
public: 
  Sample2D() : cluster(-1) {}

  // ATTENTION: don't make it unsigned (!), it can be -1
  /** id of the cluster this sample belongs to */
  int cluster;

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
