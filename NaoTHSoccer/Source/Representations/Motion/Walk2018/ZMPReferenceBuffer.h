/**
* @file MotionStatus.h
*
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Definition of the class MotionStatus
*/

#ifndef __ZMP_REF_BUFFER_h_
#define __ZMP_REF_BUFFER_h_

#include "Tools/DataStructures/Printable.h"
#include <list>
#include "Tools/Math/Vector3.h"

#include <string>

class ZMPReferenceBuffer : public naoth::Printable
{
  public:
    ZMPReferenceBuffer(){}

    std::list<double> refZMPx;
    std::list<double> refZMPy;
    std::list<double> refZMPz;

    void push(const Vector3d& zmp){
        refZMPx.push_back(zmp.x);
        refZMPy.push_back(zmp.y);
        refZMPz.push_back(zmp.z);
    }

    virtual void print(std::ostream& /*stream*/) const
    {
    }
};

//namespace naoth
//{
//  template<>
//  class Serializer<ZMPReferenceBuffer>
//  {
//  public:
//    static void serialize(const ZMPReferenceBuffer& representation, std::ostream& stream);
//    static void deserialize(std::istream& stream, ZMPReferenceBuffer& representation);
//  };
//}

#endif // __ZMP_REF_BUFFER_h_
