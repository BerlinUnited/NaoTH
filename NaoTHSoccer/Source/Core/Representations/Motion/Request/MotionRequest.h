/**
 * @file MotionRequest.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * Definition of the class MotionRequest
 */

#ifndef __MotionRequest_h_
#define __MotionRequest_h_

#include <string>

#include "Tools/DataStructures/Printable.h"

#include "MotionID.h"
#include "KickRequest.h"
#include "WalkRequest.h"

/**
 * This describes the MotionRequest
 */
class MotionRequest : public naoth::Printable 
{
public:

  MotionRequest() 
    :
    time(0),
    cognitionFrameNumber(0),
    id(motion::init),
    forced(false),
    standHeight(-1),
    calibrateFootTouchDetector(false),
    standardStand(true)
  {
  }

  ~MotionRequest() 
  {
  }

  /** timestamp when this request was generated */
  unsigned int time;

  /** the number of the cognition frame when this request was generated */
  unsigned int cognitionFrameNumber;

  /** id of the motion to be executed */
  motion::MotionID id;

  /** force the motion be executed immediately */
  bool forced;

  // high of the hip when "stand" is requested
  // if the value is < 0, then the default value is used 
  double standHeight;

  // perform calibration of the parameters for the foot touch detection
  bool calibrateFootTouchDetector;

  // go to stand after the end of the motion (is it correct?)
  bool standardStand;

  /** special parameters if kick is requested */
  KickRequest kickRequest;

  /** special parameters if walk is requested */
  WalkRequest walkRequest;


  /** set the same default values as at construction */
  void reset() 
  {
    time = 0;
    cognitionFrameNumber = 0;
    id = motion::init;
    forced = false;
    standHeight = -1;
    calibrateFootTouchDetector = false;
    standardStand = true;

    // reset by creating new once
    kickRequest = KickRequest();
    walkRequest = WalkRequest();
  }//end reset

  virtual void print(ostream& stream) const 
  {
    stream << "time = " << time << endl;
    stream << "cognition frame = " << cognitionFrameNumber << endl;
    stream << "MotionID = " << motion::getName(id) << endl;
    stream << "standardStand = " << standardStand <<endl;
    switch (id) {
      case motion::walk:
        walkRequest.print(stream);
        break;
      case motion::kick:
        kickRequest.print(stream);
        break;
      case motion::stand:
        stream << "stand height = "<<standHeight<<"\n";
        break;
      default:
        break;
    }//end switch
  }//end print
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


