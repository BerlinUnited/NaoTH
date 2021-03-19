
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
public:
  BallModel()
    :
      valid(false),
      knows(false),
      timeBallIsSeen(0)
    {}

  ~BallModel(){}

public:
  // The position of the ball relative to the robot (in mm)
  Vector2d position;
  // The speed of the ball relative to the robot (in mm/s)
  Vector2d speed;

  // the rest position
  Vector2d position_at_rest;

  // precalculated ball position in the coordinates of the planed motion (see motionStatus)
  Vector2d positionPreview;
  Vector2d positionPreviewInLFoot;
  Vector2d positionPreviewInRFoot;

  // algorithmical validity of the model: true => the ball locator was able to compute a ball model
  bool valid;

  // indicates that the ball was perceived well enough to be sure
  bool knows;

  Vector2d last_known_ball;

private:

  // time how long the ball is seen without interruption
  unsigned int timeBallIsSeen;

  // Tells when the ball was seen the last time (copied from ballPercept)
  naoth::FrameInfo frameInfoWhenBallWasSeen;


public:

  const naoth::FrameInfo& getFrameInfoWhenBallWasSeen() const {
    return frameInfoWhenBallWasSeen;
  }

  unsigned int getTimeBallIsSeen() const {
    return timeBallIsSeen;
  }

  void setFrameInfoWhenBallWasSeen(const naoth::FrameInfo& frameInfo)
  {
    if(frameInfo.getFrameNumber() != frameInfoWhenBallWasSeen.getFrameNumber()+1)
      timeBallIsSeen = 0;
    else
    {
      int diff = frameInfo.getTimeSince(frameInfoWhenBallWasSeen.getTime());
      ASSERT(diff >= 0); 
      timeBallIsSeen += static_cast<unsigned int>(diff);
    }

    frameInfoWhenBallWasSeen = frameInfo;
  }

  void reset() {
    valid = false;
    knows = false;
  }

  void resetTimeBallIsSeen() {
    timeBallIsSeen = 0;
  }

  virtual void print(std::ostream& stream) const
  {
    stream << "valid = " << valid << std::endl;
    stream << "position = " << position << std::endl;
    stream << "speed = " << speed << std::endl;
    stream << "frameInfoWhenBallWasSeen:\n" << frameInfoWhenBallWasSeen << std::endl;
  }
};

namespace naoth
{
template<>
class Serializer<BallModel>
{
  public:
  static void serialize(const BallModel& object, std::ostream& stream);
  static void deserialize(std::istream& stream, BallModel& object);
};
}

#endif // _BallModel_h_


