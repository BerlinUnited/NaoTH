/**
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 */

#ifndef _GPSDATA_H
#define	_GPSDATA_H

#include "Tools/Math/Pose3D.h"
#include "Tools/DataStructures/Printable.h"

namespace naoth
{
class GPSData : public Printable
{
public:
  virtual void print(ostream& stream) const
  {
    stream << Math::toDegrees(data.rotation.getXAngle()) << ", "
           << Math::toDegrees(data.rotation.getYAngle()) << ", "
           << Math::toDegrees(data.rotation.getZAngle()) << "\n"
           << data.translation;
  }

  Pose3D data;

};
}

#endif	/* _GPSDATA_H */

