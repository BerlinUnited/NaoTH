/**
* @file ObstacleModel.cpp
*
* @author <a href="mailto:christian.heinemann@student.hu-berlin.de">Christian Heinemann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of class ObstacleModel
*
*/

#include "ObstacleModel.h"


void ObstacleModel::print(ostream& stream) const
{
  stream << "== Obstacles ==" << endl;
  stream<<"someObstacleWasSeen = "<<(someObstacleWasSeen?"true":"false")<< std::endl;

  for(unsigned int i = 0; i < obstaclesLeft.size(); i++ )
  {
    stream << "Left Obstacle Nr." << i << endl;
    stream << obstaclesLeft[i].frameInfoObstacleWasSeen << endl;
    stream << "Distance = " << obstaclesLeft[i].distance << endl;
  }
  for(unsigned int i = 0; i < obstaclesRight.size(); i++ )
  {
    stream << "Right Obstacle Nr." << i << endl;
    stream << obstaclesRight[i].frameInfoObstacleWasSeen << endl;
    stream << "Distance = " << obstaclesRight[i].distance << endl;
  }
}//end print

ObstacleModel::ObstacleModel() : someObstacleWasSeen(false)
{
}


