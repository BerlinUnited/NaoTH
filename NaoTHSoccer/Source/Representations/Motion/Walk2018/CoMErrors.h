/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionStatus
*/

#ifndef __CoMErrors_h_
#define __CoMErrors_h_

#include "Tools/Math/Vector3.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

#include <iomanip>

class CoMErrors : public naoth::Printable
{
public:
    CoMErrors(){}

    RingBufferWithSum<double, 100> absolute2;
    RingBufferWithSum<Vector3d, 100>       e;

    const Vector3d currentComError() const {
        return e.last();
    }

    void reset(){
        absolute2.clear();
        e.clear();
    }

    virtual void print(std::ostream& stream) const
    {
        stream << std::fixed << std::setprecision(3);
        stream << "--- absolute error ---" << std::endl;
        stream << "avg^2: " << std::setw(8) << absolute2.getAverage() << std::endl;
        stream << "--- error --- " << std::endl;
        stream << "avg x: " << std::setw(8) << e.getAverage().x << std::endl;
        stream << "avg y: " << std::setw(8) << e.getAverage().y << std::endl;
        stream << "avg z: " << std::setw(8) << e.getAverage().z << std::endl;
    }
};

namespace naoth
{
  template<>
  class Serializer<CoMErrors>
  {
  public:
      static void serialize(const CoMErrors& representation, std::ostream& stream)
      {
          naothmessages::CoMErrors message;
          message.set
      }
    static void deserialize(std::istream& stream, CoMErrors& representation);
  };
}

#endif // __CoMErrors_h_
