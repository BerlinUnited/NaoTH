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
public:
  BallModel()
    :
      ballWasSeen(false),
      valid(false)
    {}

  ~BallModel(){}

  // The position of the ball relative to the robot (in mm)
  Vector2<double> position;

  Vector2<double> positionPreview;
  Vector2<double> positionPreviewInLFoot;
  Vector2<double> positionPreviewInRFoot;

  // The speed of the ball relative to the robot (in mm/s)
  Vector2<double> speed;

  // Tells if the ball was seen in the last frame or if not
  bool ballWasSeen;
  FrameInfo frameInfoWhenBallWasSeen;

  // 
  bool valid;

  /** The estimated position of the ball 0-10s in the future */
  Vector2<double> futurePosition[BALLMODEL_MAX_FUTURE_SECONDS+1];

  void reset()
  {
    ballWasSeen = false;
    valid = false;
  }

  virtual void print(ostream& stream) const
  {
    stream << "valid = " << valid << endl;
    stream << "ballWasSeen = " << ballWasSeen << endl;
    stream << "position = " << position << endl;
    stream << "speed = " << speed << endl;
    stream << "frameInfoWhenBallWasSeen:\n" << frameInfoWhenBallWasSeen << endl;
  }//end print
};

#endif// __BallModel_h_
