/**
* @file ObstacleModel.cpp
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Stella Alice</a>
* Implementation of class ObstacleModel
*
*/

#include "ObstacleModel.h"
#include <limits>

Obstacle::Obstacle()
  : center(Vector2d()),
    shape_points(Math::Polygon<double>())
{
}


void ObstacleModel::print(std::ostream& stream) const
{
  stream << "== Obstacles ==" << std::endl;
  stream<<"leftDistance = "<< leftDistance << std::endl;
  stream<<"rightDistance = "<< rightDistance << std::endl;
  stream<<"frontDistance = "<< frontDistance << std::endl;
  stream<<"blockedTime = "<< blockedTime << std::endl;
}

ObstacleModel::ObstacleModel()
  : leftDistance(std::numeric_limits<double>::max()),
    rightDistance(std::numeric_limits<double>::max()),
    frontDistance(std::numeric_limits<double>::max()),
    blockedTime(-1)
{
}


