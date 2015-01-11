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

  WalkRequest():
    character(0.5),
    coordinate(Hip)
  {};

  ~WalkRequest(){}

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
  
  struct StepControlRequest
  {
    StepControlRequest()
      :
      stepID(0),
      moveLeftFoot(false),
      time(0),
      speedDirection(0),
      scale(1.0)
    {}

    unsigned int stepID; // it should match the current step id in walk, otherwise it will not be accepted
    bool moveLeftFoot; // it should also match
    Pose2D target; // in coordinate
    unsigned int time; // in ms
    double speedDirection; //TODO: what is that?
    double scale;
  };

  // indicates speed and stability, in range [0, 1]
  // 0: stablest (slow)
  // 0.5: normal
  // 1: fastest (unstable)
  double character;

  //
  Coordinate coordinate;

  // 
  Pose2D target;

  //
  StepControlRequest stepControl;

  // offset of relation between left foot and body
  Pose2D offset;


  void print(std::ostream& stream) const
  {
    stream << "target: " << target << std::endl;
    stream << "coordinate: "<< getCoordinateName(coordinate) << std::endl;
    stream << "character: " << character << std::endl;
    stream << "offset: "<< offset << "\n";

    stream << "step control: "<< stepControl.stepID <<" "
           << (stepControl.moveLeftFoot?"L":"R")<<" "<<stepControl.time
           <<"\n step target: "<< stepControl.target<<"\n"
           <<"speed direction:"<< Math::toDegrees(stepControl.speedDirection) << std::endl;

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

