/** 
 * @file BallCandidates.h
 *
 * Declaration of class BallCandidates
 */

#ifndef _BallCandidates_h_
#define _BallCandidates_h_

#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <list>
#include <vector>

class BallCandidates : public naoth::Printable
{
public:
  BallCandidates(){}

  class Patch 
  {
  public:
    static const int SIZE = 12;
    Patch(int) : data(SIZE*SIZE) 
    {}
    Vector2i min;
    Vector2i max;
    std::vector<unsigned char> data;
  };

  typedef std::list<Patch> PatchesList;
  PatchesList patches;

  Patch& nextFreePatch() {
    patches.emplace_back(0);
    return patches.back();
  }

  void reset()
  {
    patches.clear();
  }

  virtual void print(std::ostream& /*stream*/) const
  {

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


