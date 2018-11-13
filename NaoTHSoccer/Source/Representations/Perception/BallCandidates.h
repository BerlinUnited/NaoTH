/** 
 * @file BallCandidates.h
 *
 * Declaration of class BallCandidates
 */

#ifndef _BallCandidates_h_
#define _BallCandidates_h_

#include <Tools/Math/Vector2.h>
#include <Tools/ImageProcessing/ImagePrimitives.h>
//#include <Tools/ColorClasses.h>

#include <Tools/DataStructures/Serializer.h>

#include <list>
#include <vector>

class BallCandidates
{
public:
  BallCandidates(){}

  class ClassifiedPixel
  {
  public:
    Pixel pixel;
    unsigned char c; // ColorClasses::Color, NOTE: sizeof(ColorClasses::Color) == 4
  };

  template<typename T>
  class PatchT 
  {
  public:
    static const int SIZE = 16;
    PatchT() : data(SIZE*SIZE)
    {}
    //NOTE: dummy constructor to make emplace_back work
    PatchT(int) : data(SIZE*SIZE)
    {}
    PatchT(const Vector2i& min, const Vector2i& max) : 
      min(min), 
      max(max),
      data(SIZE*SIZE)
    {}
    Vector2i min;
    Vector2i max;
    std::vector<T> data;
  };

  typedef PatchT<unsigned char> Patch;
  typedef PatchT<Pixel> PatchYUV;
  typedef PatchT<ClassifiedPixel> PatchYUVClassified;

  typedef std::list<Patch> PatchesList;
  PatchesList patches;

  typedef std::list<PatchYUV> PatchesYUVList;
  PatchesYUVList patchesYUV;

  typedef std::list<PatchYUVClassified> PatchYUVClassifiedList;
  PatchYUVClassifiedList patchesYUVClassified;

  Patch& nextFreePatch() {
    patches.emplace_back(0);
    return patches.back();
  }

  PatchYUV& nextFreePatchYUV() {
    patchesYUV.emplace_back(0);
    return patchesYUV.back();
  }

  PatchYUVClassified& nextFreePatchYUVClassified() {
    patchesYUVClassified.emplace_back(0);
    return patchesYUVClassified.back();
  }

  void reset()
  {
    patches.clear();
    patchesYUV.clear();
    patchesYUVClassified.clear();
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


