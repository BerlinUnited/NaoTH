
/**
* @file BallModel.h
*
* @author <a href="mailto:goehring@informatik.hu-berlin.de">Daniel Goehring</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BallModel
*/

#ifndef __BallModel_h_
#define __BallModel_h_

#include "Tools/Math/Vector2.h"

#include "Representations/Infrastructure/FrameInfo.h"
#include <Tools/DataStructures/Printable.h>

#define BALLMODEL_MAX_FUTURE_SECONDS 10

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

  // The position of the ball relative to the robot (in mm)
  Vector2<double> position;

  Vector2<double> positionPreview;
  Vector2<double> positionPreviewInLFoot;
  Vector2<double> positionPreviewInRFoot;

  /** The estimated position of the ball 0-10s in the future */
  Vector2<double> futurePosition[BALLMODEL_MAX_FUTURE_SECONDS+1];

  // The speed of the ball relative to the robot (in mm/s)
  Vector2<double> speed;

  // Tells when the ball was seen the last time
  const naoth::FrameInfo& frameInfoWhenBallWasSeen;
  // time how long the ball is seen without interruption
  const unsigned int& timeBallIsSeen;
  bool valid;


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

  void reset()
  {
    valid = false;
  }

  virtual void print(std::ostream& stream) const
  {
    stream << "valid = " << valid << std::endl;
    stream << "position = " << position << std::endl;
    stream << "speed = " << speed << std::endl;
    stream << "frameInfoWhenBallWasSeen:\n" << frameInfoWhenBallWasSeen << std::endl;
  }//end print
};

#endif// __BallModel_h_


