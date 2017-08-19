/**
 * @file BPathPlanner.h
 *
 * @author <a href="mailto:akcayyig@hu-berlin.de">Yigit Can Akcay</a>
 * Definition of class BPathPlanner (BISEC)
 */

#ifndef _BPathPlanner_H_
#define _BPathPlanner_H_

#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

class BPathPlanner
{
public:
  BPathPlanner();
  ~BPathPlanner();

  Vector2d get_gait(const Vector2d& goal,
                    const Vector3d& obstacles) const;

private:

  struct Trajectory
  {
    Vector2d start;
    Vector2d end;
  };

  Trajectory compute_path(const Vector2d& goal,
                          const unsigned int depth,
                          const int sign) const;
  
};

#endif /* _BPathPlanner_H_ */
