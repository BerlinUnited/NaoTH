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
  private:
    size_t _size;
    
  public:
    PatchT() : _size(0)
    {}

    PatchT(size_t size) : 
      _size(size), 
      data(size*size)
    {}

    PatchT(const Vector2i& min, const Vector2i& max) : 
      _size(0),
      min(min), 
      max(max)
    {}

    PatchT(const Vector2i& min, const Vector2i& max, size_t size) :
      _size(size),
      min(min), 
      max(max),
      data(size*size)
    {}

    void setSize(size_t size) {
      _size = size;
      data.resize(size*size);
    }

    size_t size() const {
      return _size;
    }

    Vector2i center() const {
      return Vector2i((min.x + max.x)/2, (min.y + max.y)/2);
    }

    double radius() const {
      return static_cast<double>(max.x - min.x)/2.0;
    }

    int width() const {
      return max.x - min.x;
    }

    int height() const {
      return max.y - min.y;
    }

  public:
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
    patches.emplace_back();
    return patches.back();
  }

  PatchYUV& nextFreePatchYUV() {
    patchesYUV.emplace_back();
    return patchesYUV.back();
  }

  PatchYUVClassified& nextFreePatchYUVClassified() {
    patchesYUVClassified.emplace_back();
    return patchesYUVClassified.back();
  }

  void reset()
  {
    patches.clear();
    patchesYUV.clear();
    patchesYUVClassified.clear();
  }

  virtual ~BallCandidates() {}
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


