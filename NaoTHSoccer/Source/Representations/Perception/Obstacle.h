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

#endif // OBSTACLE_H
