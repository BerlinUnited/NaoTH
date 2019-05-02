#ifndef NOGREENOBSTACLE_H
#define NOGREENOBSTACLE_H

class NoGreenObstacle {
public:
  NoGreenObstacle() : valid(false) {}

  bool valid;

  double no_green_density;

  double no_green_density_left;
  double no_green_density_right;
};

#endif // NOGREENOBSTACLE_H
