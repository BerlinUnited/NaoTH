/**
* @file WalkRequest.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class Cognition
*/

#ifndef __WalkRequest_h_
#define __WalkRequest_h_

#include <string>
#include <Tools/Math/Pose2D.h>
#include <Tools/DataStructures/Printable.h>
#include <Messages/Representations.pb.h>
#include <Tools/DataStructures/Serializer.h>

/**
* This describes the WalkRequest
*/
class WalkRequest : public naoth::Printable
{
public:

  enum Coordinate{
    Hip,
    LFoot,
    RFoot,
    numOfCoordinate
  };

  /** constructor */
  WalkRequest():
    coordinate(Hip)
  {};
  ~WalkRequest(){};

  static std::string getCoordinateName(Coordinate c)
  {
    switch(c)
    {
      case Hip: return "hip";
      case LFoot: return "left_foot";
      case RFoot: return "right_foot";
      default: return "unknown";
    }
  }//end getCoordinateName
  
  Coordinate coordinate;
  Pose2D target;

  void print(ostream& stream) const
  {
    stream << "target: " << target << endl;
    stream << "coordinate: "<< getCoordinateName(coordinate) <<endl;
  }//end print
  
};

namespace naoth
{
  template<>
  class Serializer<WalkRequest>
  {
  public:
    static void serialize(const WalkRequest& representation, std::ostream& stream);
    static void serialize(const WalkRequest& representation, naothmessages::WalkRequest* msg);
    static void deserialize(std::istream& stream, WalkRequest& representation);
    static void deserialize(const naothmessages::WalkRequest* msg, WalkRequest& representation);
  };
}


#endif // __WalkRequest_h_

