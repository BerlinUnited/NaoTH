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
  stream<<"leftDistance = "<< leftDistance << std::endl;
  stream<<"rightDistance = "<< rightDistance << std::endl;
  stream<<"frontDistance = "<< frontDistance << std::endl;
}//end print

ObstacleModel::ObstacleModel()
{
}


