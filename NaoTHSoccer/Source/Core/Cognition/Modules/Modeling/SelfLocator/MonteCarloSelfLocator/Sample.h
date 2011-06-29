/**
* @file Sample.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Sample
*/

#ifndef __Sample_h_
#define __Sample_h_

#include "Tools/Math/Pose2D.h"

class Sample: public Pose2D
{
public: 
  Sample() : cluster(0), likelihood(0.0){}
  ~Sample() {}

  int cluster; /** id of the cluster this sample belongs to */
  double likelihood;


};

#endif //__Sample_h_
