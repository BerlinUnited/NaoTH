#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <vector>
#include <Tools/Math/Vector2.h>

class UltrasonicObstaclePercept {
public:
    // location of the distance measurement projected on the ground
    std::vector<Vector2d> location_on_ground;
};

class VisionObstacle {
public:
  VisionObstacle() : valid(false) {}

  bool valid;

  bool isOccupied;

  bool onTheRight;
  bool onTheLeft;
};

class VisionObstaclePreview : public VisionObstacle {};

class ObstacleBehindBall : public VisionObstacle {};

#endif // OBSTACLE_H
