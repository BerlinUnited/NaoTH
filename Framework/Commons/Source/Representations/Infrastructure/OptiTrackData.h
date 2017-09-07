/**
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _OptiTrackData_H_
#define _OptiTrackData_H_

#include "Tools/Math/Pose3D.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"
#include <map>  

namespace naoth
{

class OptiTrackData : public naoth::Printable
{
public:
  virtual void print(std::ostream& stream) const
  {
    for(const auto& element : trackables) 
    {
      stream << element.first << ":" << std::endl;
      stream << "\ttranslation (mm): " << element.second.translation << std::endl;
      stream << "\trotation (deg): " 
        << Math::toDegrees(element.second.rotation.getXAngle()) << ", "
        << Math::toDegrees(element.second.rotation.getYAngle()) << ", "
        << Math::toDegrees(element.second.rotation.getZAngle()) << std::endl;
    }
  }
  
  std::map<std::string,Pose3D> trackables;
};

template<> 
class Serializer<OptiTrackData>
{
  public:
  static void serialize(const OptiTrackData& representation, std::ostream& stream);
  static void deserialize(std::istream& stream, OptiTrackData& representation);
};

}

#endif  /* _OptiTrackData_H */

