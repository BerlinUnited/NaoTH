#ifndef OBSTACLE_H
#define OBSTACLE_H

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
