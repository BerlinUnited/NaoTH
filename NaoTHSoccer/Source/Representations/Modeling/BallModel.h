
/**
* @file BallModel.h
*
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BallModel
*/

#ifndef _BallModel_h_
#define _BallModel_h_

#include "Tools/Math/Vector2.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include <Tools/DataStructures/Printable.h>
#include <vector>

class BallModel : public naoth::Printable
{
private:

  unsigned int _timeBallIsSeen;
  naoth::FrameInfo _frameInfoWhenBallWasSeen;

public:
  BallModel()
    :
      _timeBallIsSeen(0),

      // accessors
      frameInfoWhenBallWasSeen(_frameInfoWhenBallWasSeen),
      timeBallIsSeen(_timeBallIsSeen),

      valid(false)
    {}

  ~BallModel(){}

public:
  // The position of the ball relative to the robot (in mm)
  Vector2d position;

  Vector2d positionPreview;
  Vector2d positionPreviewInLFoot;
  Vector2d positionPreviewInRFoot;

  /** The estimated position of the ball 0-10s in the future */
  std::vector<Vector2d> futurePosition;

  // The speed of the ball relative to the robot (in mm/s)
  Vector2d speed;

  // Tells when the ball was seen the last time
  const naoth::FrameInfo& frameInfoWhenBallWasSeen;
  // time how long the ball is seen without interruption
  const unsigned int& timeBallIsSeen;
  bool valid;


public:

  void setFrameInfoWhenBallWasSeen(const naoth::FrameInfo& frameInfo)
  {
    if(frameInfo.getFrameNumber() != frameInfoWhenBallWasSeen.getFrameNumber()+1)
      _timeBallIsSeen = 0;
    else
    {
      ASSERT(frameInfoWhenBallWasSeen.getTime() < frameInfo.getTime());
      _timeBallIsSeen += frameInfo.getTimeSince(frameInfoWhenBallWasSeen.getTime());
    }

    _frameInfoWhenBallWasSeen = frameInfo;
  }//end setFrameInfoWhenBallWasSeen

  void reset() {
    valid = false;
  }

  void resetTimeBallIsSeen() {
    _timeBallIsSeen = 0;
  }

  virtual void print(std::ostream& stream) const
  {
    stream << "valid = " << valid << std::endl;
    stream << "position = " << position << std::endl;
    stream << "speed = " << speed << std::endl;
    stream << "frameInfoWhenBallWasSeen:\n" << frameInfoWhenBallWasSeen << std::endl;
  }
};

#endif // _BallModel_h_


