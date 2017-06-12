/**
 * @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Benjamin</a>
 */

#ifndef _GPSDATA_H
#define _GPSDATA_H

#include "Tools/Math/Pose3D.h"
#include "Tools/DataStructures/Printable.h"
#include <Tools/DataStructures/Serializer.h>

namespace naoth
{

class GPSData : public naoth::Printable
{
public:
  //TODO: change that
  virtual void print(std::ostream& stream) const
  {
    stream << Math::toDegrees(data.rotation.getXAngle()) << ", "
           << Math::toDegrees(data.rotation.getYAngle()) << ", "
           << Math::toDegrees(data.rotation.getZAngle()) << "\n"
           << data.translation;
  }

  Pose3D data;

};


  template<>
  class Serializer<GPSData>
  {
  public:
    static void serialize(const GPSData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, GPSData& representation);
  };


}

#endif  /* _GPSDATA_H */

