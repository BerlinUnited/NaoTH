#ifndef OBSTACLE_H
#define OBSTACLE_H

class VisionObstacle {
public:
  VisionObstacle() : valid(false) {}

  bool valid;

  bool inFront;

  bool onTheRight;
  bool onTheLeft;
};

class VisionObstaclePreview : public VisionObstacle {};

#endif // OBSTACLE_H
