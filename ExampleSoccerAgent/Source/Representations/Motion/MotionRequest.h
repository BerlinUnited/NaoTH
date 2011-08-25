/**
 * @file MotionRequest.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Definition of the class MotionRequest
 */

#ifndef __MotionRequest_h_
#define __MotionRequest_h_

#include <string>
#include <Tools/DataStructures/Serializer.h>
#include <Tools/DataStructures/Printable.h>

/**
 * This describes the MotionRequest
 */
class MotionRequest : public naoth::Printable
{
public:

  /** constructor */
  MotionRequest()
  {
  }

  ~MotionRequest() {
  }

  enum MotionID
    {
      stand,
      walk_forward,
      turn_left,
      stand_up_from_front,
      stand_up_from_back,
      numOfMotionID
    };

  /** id of the motion to be executed */
  MotionID id;

  virtual void print(ostream& stream) const
  {
    stream << "motionID: " << (int) id;
  }

};

namespace naoth
{
  template<>
  class Serializer<MotionRequest>
  {
  public:
    static void serialize(const MotionRequest& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, MotionRequest& representation);
  };
}

#endif // __MotionRequest_h_


