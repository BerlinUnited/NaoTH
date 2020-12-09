/**
* @file WalkRequest.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class Cognition
*/

#ifndef _WalkRequest_h_
#define _WalkRequest_h_

#include <string>
#include <Tools/Math/Pose2D.h>
#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

// NOTE: this has to be here because of the special serialize methods
#include <Messages/Representations.pb.h>

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
      type(KICKSTEP),       // KICKSTEP instead of WALKSTEP because of backwards compatibility
      stepID(0),
      moveLeftFoot(false),
      time(0),
      speedDirection(0),
      scale(1.0),
      restriction(RestrictionMode::HARD),   
      isProtected(false),
      stepRequestID(0)
    {}

    enum StepType {
      WALKSTEP,
      KICKSTEP,
      ZEROSTEP
    };
    StepType type;
    unsigned int stepID; // it should match the current step id in walk, otherwise it will not be accepted
    bool moveLeftFoot; // it should also match, false = rightfoot
    Pose2D target; // in coordinate
    unsigned int time; // in ms
    //angle (in radiant) of the speed of the foot at the end of the controlled step
    //e.g., kick to left => speedDirection = Math::toRadiant(-90)
    double speedDirection;
    // time scale for the step trajectory (0..1], 
    // e.g., scale = 1 => normal step trajectory, scale < 1 => faster step
    double scale;

    enum RestrictionMode { HARD, SOFT };
    RestrictionMode restriction; // additionally to restrictStepSize use restrictStepChange ?
    bool isProtected;            // determine if the request is protected from stabilization of any sort
    unsigned int stepRequestID;  // this ID is for stepControl request PathPlanning
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
    stream << "type: " << (stepControl.type == StepControlRequest::StepType::WALKSTEP ? "WALKSTEP" : "KICKSTEP") << std::endl;
    stream << "target: " << target << std::endl;
    stream << "coordinate: "<< getCoordinateName(coordinate) << std::endl;
    stream << "character: " << character << std::endl;
    stream << "offset: "<< offset << "\n";

    stream << "step control: "<< stepControl.stepID <<" "
           << (stepControl.moveLeftFoot?"L":"R")<<" "<<stepControl.time
           <<"\n step target: "<< stepControl.target<<"\n"
           <<"speed direction:"<< Math::toDegrees(stepControl.speedDirection) << std::endl;

  }//end print
  
  void assertValid() const {
    ASSERT(character <= 1);
    ASSERT(character >= 0);
    ASSERT(!Math::isNan(target.translation.x));
    ASSERT(!Math::isNan(target.translation.y));
    ASSERT(!Math::isNan(target.rotation));
  }
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


#endif // _WalkRequest_h_

