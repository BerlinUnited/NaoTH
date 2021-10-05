/**
* @file ObstacleModel.h
*
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Benjamin</a>
* Definition of class ObstacleModel
*/

#ifndef _ObstacleModel_h_
#define _ObstacleModel_h_

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Polygon.h"

class Obstacle{
  public:
    Obstacle();
    Vector2d center;
    Math::Polygon<double> shape_points;
};

class ObstacleModel : public naoth::Printable
{
public:
  ObstacleModel();
  
  std::vector<Obstacle> obstacleList;

  // old stuff for ultrasonic detector should be 
  double leftDistance;
  double rightDistance;
  double frontDistance;
  int blockedTime;

  virtual void print(std::ostream& stream) const;
};

#endif// _ObstacleModel_h_
