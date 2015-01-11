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
        LEFT = 1,
        RIGHT = 2,
        DOUBLE_LEFT = 4,
        DOUBLE_RIGHT = 8,
        DOUBLE = 16,
        NONE = 0,
    };

  SupportPolygon();
  ~SupportPolygon();
  
  bool isLeftFootSupportable() const;
  
  bool isRightFootSupportable() const;

  std::vector<Vector2<double> > vertex;
  SupportMode mode;
  Vector3<double> forceCenter;
  Pose3D standOrigin;
};

std::ostream& operator <<(std::ostream& ost, const SupportPolygon& v);

#endif  /* _SUPPORTPOLYGON_H */

