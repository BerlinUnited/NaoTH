/**
* @file KickRequest.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of the class Cognition
*/

#ifndef __KickRequest_h_
#define __KickRequest_h_

#include "Tools/DataStructures/Printable.h"

/**
* This describes the KickRequest
*/
class KickRequest : public naoth::Printable
{
public:
  /** constructor */
  KickRequest() :
      kickDirection(0),
      kickFoot(right),
      kickType(front),
      finishKick(false)
  {};
  ~KickRequest(){};

  enum KickFootID
  {
    left = 1,
    none = 0,
    right = -1
  };

  // kick types
  enum KickTypeID
  {
    front,
    inner,
    outer
  };

  Vector3<double> kickPoint; // it is the position of ball in robot's local coordination
  double kickDirection;

  KickFootID kickFoot;

  KickTypeID kickType;
  
  // if a kick is already prepared it will 
  // execute the actual kicking motion
  bool finishKick;

  void print(ostream& stream) const
  {
    stream << "kickPoint = " << kickPoint << endl;
    stream << "direction = " << Math::toDegrees(kickDirection) << endl;
  }//end print
};

#endif // __MotionRequest_h_
