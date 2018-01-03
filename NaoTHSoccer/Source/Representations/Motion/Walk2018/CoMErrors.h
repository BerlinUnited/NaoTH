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

class CoMErrors : public naoth::Printable
{
public:
    CoMErrors(){}

    RingBufferWithSum<double, 100> absolute2;
    RingBufferWithSum<Vector3d, 100>       e;

    const Vector3d currentComError() const {
        return e.last();
    }

    virtual void print(std::ostream& /*stream*/) const
    {
    }
};

//namespace naoth
//{
//  template<>
//  class Serializer<CoMErrors>
//  {
//  public:
//    static void serialize(const CoMErrors& representation, std::ostream& stream);
//    static void deserialize(std::istream& stream, CoMErrors& representation);
//  };
//}

#endif // __CoMErrors_h_
