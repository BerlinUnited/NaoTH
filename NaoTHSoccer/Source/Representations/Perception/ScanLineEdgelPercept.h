/** 
 * @file ScanLineEdgelPercept.h
 *
 * Declaration of class ScanLineEdgelPercept
 */

#ifndef _ScanLineEdgelPercept_h_
#define _ScanLineEdgelPercept_h_

#include "Tools/Math/Vector2.h"
#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/ColorClasses.h"

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <vector>

class ScanLineEdgelPercept : public naoth::Printable
{ 
public:
  ScanLineEdgelPercept(){}

  class EndPoint
  {
  public:
    EndPoint() : color(ColorClasses::none), ScanLineID(0), valid(false), greenFound(false) {}
    Vector2i posInImage;
    Vector2d posOnField;
    ColorClasses::Color color;
    unsigned int ScanLineID;
    bool valid;
    bool greenFound;
  };

  class EdgelPair : public EdgelT<double>
  {
  public:
    EdgelPair() : begin(-1), end(-1), id(-1), adaptive(false){}
    int begin;
    int end;
    int id;
    bool adaptive;
  };

  std::vector<EdgelPair> pairs; // this depends on edgels
  std::vector<Edgel> edgels;

  /** */
  std::vector<EndPoint> endPoints;

  void reset()
  {
    endPoints.clear();
    pairs.clear();
    edgels.clear();
  }

  virtual void print(std::ostream& /*stream*/) const
  {
    // TODO: do we need it?
  }
};

class ScanLineEdgelPerceptTop : public ScanLineEdgelPercept
{
public:
  virtual ~ScanLineEdgelPerceptTop() {}
};

namespace naoth
{
  template<>
  class Serializer<ScanLineEdgelPercept>
  {
  public:
    static void serialize(const ScanLineEdgelPercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, ScanLineEdgelPercept& representation);
  };

  template<>
  class Serializer<ScanLineEdgelPerceptTop> : public Serializer<ScanLineEdgelPercept>
  {};

}

#endif //_ScanLineEdgelPercept_h_
