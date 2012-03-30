/**
 * @file InertialModel.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef _InertialModel_H_
#define _InertialModel_H_

#include <string>

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"

class InertialModel: public naoth::Printable
{
public:
  Vector2<double> orientation;
  
  virtual void print(ostream& stream) const
  {
    stream << "orientation:" << Math::toDegrees(orientation.x) << " " << Math::toDegrees(orientation.y) << "\n";
  }
};

namespace naoth
{
  template<>
  class Serializer<InertialModel>
  {
  public:
    static void serialize(const InertialModel& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, InertialModel& representation);
  };
}

#endif // _InertialModel_H_
