/**
* @file ObstacleModel.h
*
* @author <a href="mailto:borisov@informatik.hu-berlin.de">Alexander Borisov</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of class ObstacleModel
*/

#ifndef __ObstacleModel_h_
#define __ObstacleModel_h_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "Tools/ColorClasses.h"

#include "Tools/DataStructures/Printable.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "RobotPose.h"

class ObstacleModel : public Printable
{
public:
  struct Obstacle
  {
    Obstacle():distance(0){}

    FrameInfo frameInfoObstacleWasSeen;
    double distance;
  };
  ObstacleModel();

  vector<Obstacle> obstaclesLeft;
  vector<Obstacle> obstaclesRight;

  double nearestObstacleDistance;

  bool someObstacleWasSeen;
  FrameInfo frameWhenObstacleWasSeen;

  virtual void print(ostream& stream) const;
};//end class ObstacleModel


#endif// __ObstacleModel_h_
