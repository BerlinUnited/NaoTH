/** 
 * @file BallCandidates.h
 *
 * Declaration of class BallCandidates
 */

#ifndef _BallCandidates_h_
#define _BallCandidates_h_

#include <Tools/Math/Vector2.h>
#include <Tools/ImageProcessing/ImagePrimitives.h>

#include <Tools/DataStructures/Serializer.h>

#include <list>
#include <vector>

class BallCandidates
{
public:
  BallCandidates(){}

  template<typename T>
  class PatchT 
  {
  public:
    static const int SIZE = 12;
    PatchT(int) : data(SIZE*SIZE) 
    {}
    Vector2i min;
    Vector2i max;
    std::vector<T> data;
  };

  typedef PatchT<unsigned char> Patch;
  typedef PatchT<Pixel> PatchYUV;

  typedef std::list<Patch> PatchesList;
  PatchesList patches;

  typedef std::list<PatchYUV> PatchesYUVList;
  PatchesYUVList patchesYUV;

  Patch& nextFreePatch() {
    patches.emplace_back(0);
    return patches.back();
  }

  void reset()
  {
    patches.clear();
    patchesYUV.clear();
  }
};

class BallCandidatesTop : public BallCandidates
{
public:
  virtual ~BallCandidatesTop() {}
};

namespace naoth
{
  template<>
  class Serializer<BallCandidates>
  {
  public:
    static void serialize(const BallCandidates& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, BallCandidates& representation);
  };

  template<>
  class Serializer<BallCandidatesTop> : public Serializer<BallCandidates>
  {};
}


#endif //_BallCandidates_h_


