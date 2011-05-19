/**
* @file WalkRequest.h
*
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* Definition of the class Cognition
*/

#ifndef __WalkRequest_h_
#define __WalkRequest_h_

#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/Printable.h"

/**
* This describes the WalkRequest
*/
class WalkRequest : public Pose2D, public naoth::Printable
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
    coordinate(Hip),
    stopWithStand(true),
    stable(false)
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
  bool stopWithStand; // Should the robot keep the standard stand pose or not after walking

  bool stable;

  void print(ostream& stream) const
  {
    stream << "x = " << translation.x << endl;
    stream << "y = " << translation.y << endl;
    stream << "rotation = " << rotation << endl;
    stream << "coordinate: "<< getCoordinateName(coordinate) <<endl;
    if ( stopWithStand )
    {
      stream << " stop with stand " <<endl;
    }
    else
    {
      stream <<" stop without stand "<<endl;
    }
  }//end print
  
};

#endif // __WalkRequest_h_

