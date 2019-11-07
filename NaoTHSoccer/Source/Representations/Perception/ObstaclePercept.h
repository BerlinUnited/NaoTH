#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"

#include <vector>
#include <iomanip>

class UltrasonicObstaclePercept : public naoth::Printable {
public:
    // location of the distance measurement projected on the ground
    std::vector<Vector2d> location_on_ground;

    virtual void print(std::ostream& stream) const {
        stream << std::fixed << std::setprecision(4);
        stream << "location (x[mm] y[mm]) on the ground in local coordinates" << std::endl;
        for(auto& location : location_on_ground) {
            stream << std::setw(9) << location.x
                   << "   " << std::setw(9) << location.y << std::endl;
        }
    }
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
