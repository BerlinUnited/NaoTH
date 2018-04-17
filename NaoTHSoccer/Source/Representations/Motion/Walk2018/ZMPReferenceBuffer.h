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
  private:
    std::list<double> refZMPx;
    std::list<double> refZMPy;
    std::list<double> refZMPz;

  public:
    ZMPReferenceBuffer(){}

    const std::list<double>& getXRef(){
        return refZMPx;
    }

    const std::list<double>& getYRef(){
        return refZMPy;
    }

    void pop() {
      refZMPx.pop_front();
      refZMPy.pop_front();
      refZMPz.pop_front();
    }

    void push(const Vector3d& zmp) {
        refZMPx.push_back(zmp.x);
        refZMPy.push_back(zmp.y);
        refZMPz.push_back(zmp.z);
    }

    Vector3d front() const {
      return Vector3d(refZMPx.front(), refZMPy.front(), refZMPz.front());
    }

    Vector3d back() const {
      return Vector3d(refZMPx.back(), refZMPy.back(), refZMPz.back());
    }

    void reset() {
      refZMPx.clear();
      refZMPy.clear();
      refZMPz.clear();
    }

    Vector3<size_t> size(){
        return Vector3<size_t>(refZMPx.size(),refZMPy.size(),refZMPz.size());
    }

    virtual void print(std::ostream& /*stream*/) const {
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
