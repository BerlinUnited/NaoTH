/**
* @file HeadMotionRequest.h
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* Definition of HeadMotionReqeuest
*/

#ifndef __HeadMotionRequest_h_
#define __HeadMotionRequest_h_

#include "HeadMotionRequestID.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"

/**
* This describes the HeadMotionRequest
*/
class HeadMotionRequest : public PlatformInterchangeable, public Printable
{
public:
  /** constructor */
  HeadMotionRequest()
    :
    id(HeadMotionRequestID::hold),
    cameraID(CameraInfo::Bottom),
    headSearchCenter(0,0,0),
    headSearchSize(90,45,0),
    headSearchDirection(true)
  {
  }

  ~HeadMotionRequest(){}

  /** id of the motion to be executed */
  HeadMotionRequestID::HeadMotionID id;

  CameraInfo::CameraID cameraID;


  // describes the target angle-position: x=yaw, y=pitch 
  Vector2<double> targetPosition;


  Vector2<double> targetPointInImage;
  Vector3<double> targetPointInTheWorld;
  Vector2<double> targetPointOnTheGround;

  // parameters for head search motion
  Vector3<double> headSearchCenter;
  Vector3<double> headSearchSize;
  bool headSearchDirection;
  


  string getCurrentRequest()
  {
    return HeadMotionRequestID::getName(id);
  }//end getCurrentRequest
  

  virtual void print(ostream& stream) const
  {
    stream << "Current MotionRequest = " << HeadMotionRequestID::getName(id) << endl;
  }//end print
};


#endif // __HeadMotionRequest_h_
