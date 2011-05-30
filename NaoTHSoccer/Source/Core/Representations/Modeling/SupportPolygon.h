/** 
* @file SupportPolygon.h
* Declaration of class SupportPolygon.
*
* @author <A href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</A>
*/

#ifndef _SUPPORTPOLYGON_H
#define  _SUPPORTPOLYGON_H

#include <ostream>
#include <vector>
#include "Tools/Math/Vector3.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose3D.h"

class SupportPolygon {

public:

    enum SupportMode
    {
        LEFT,
        RIGHT,
        DOUBLE_LEFT,
        DOUBLE_RIGHT,
        DOUBLE,
        NONE,
    };

  SupportPolygon();
  ~SupportPolygon();

  std::vector<Vector2<double> > vertex;
  SupportMode mode;
  Vector3<double> forceCenter;
  Pose3D standOrigin;
};

std::ostream& operator <<(std::ostream& ost, const SupportPolygon& v);

#endif  /* _SUPPORTPOLYGON_H */

