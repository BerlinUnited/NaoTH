/**
* @file KickRequest.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of the class Cognition
*/

#ifndef __KickRequest_h_
#define __KickRequest_h_

#include <Tools/Math/Vector3.h>

#include "Tools/DataStructures/Printable.h"
#include <Messages/Representations.pb.h>
#include <Tools/DataStructures/Serializer.h>

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
      finishKick(false)
  {};

  ~KickRequest(){};

  enum KickFootID
  {
    left = 1,
    right = -1
  };

  Vector3<double> kickPoint; // it is the position of ball in robot's local coordination
  double kickDirection;

  KickFootID kickFoot;
  
  // if a kick is already prepared it will 
  // execute the actual kicking motion
  bool finishKick;

  void print(ostream& stream) const
  {
    stream << "kickPoint = " << kickPoint << endl;
    stream << "direction = " << Math::toDegrees(kickDirection) << endl;
  }//end print
};


namespace naoth
{
  template<>
  class Serializer<KickRequest>
  {
  public:
    static void serialize(const KickRequest& representation, std::ostream& stream);
    static void serialize(const KickRequest& representation, naothmessages::KickRequest* msg);
    static void deserialize(std::istream& stream, KickRequest& representation);
    static void deserialize(const naothmessages::KickRequest* msg, KickRequest& representation);
  };
}

#endif // __MotionRequest_h_
