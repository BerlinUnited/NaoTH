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
    EndPoint():color(ColorClasses::none), ScanLineID(0), valid(false){}
    Vector2i posInImage;
    Vector2d posOnField;
    ColorClasses::Color color;
    unsigned int ScanLineID;
    bool valid;
  };

  class EdgelPair : public EdgelT<double>
  {
  public:
    EdgelPair() : begin(-1), end(-1), id(-1) {}
    int begin;
    int end;
    int id;
  };

  /** */
  std::vector<DoubleEdgel> scanLineEdgels;

  std::vector<EdgelPair> pairs;
  std::vector<Edgel> edgels;

  /** */
  std::vector<EndPoint> endPoints;

  void reset()
  {
    endPoints.clear();
    pairs.clear();
    edgels.clear();
    scanLineEdgels.clear();
  }

  virtual void print(std::ostream& stream) const
  {
    stream << "ScanLine Edgels:" << std::endl << "------" << std::endl;
    for(size_t i = 0; i < scanLineEdgels.size(); i++)
    {
      stream << "Edgel " << i << std::endl;
      stream << "  Begin = " << scanLineEdgels[i].begin << " angle = " << scanLineEdgels[i].begin_angle << std::endl;
      stream << "  Center = " << scanLineEdgels[i].center << " angle = " << scanLineEdgels[i].center_angle << std::endl;
      stream << "  End = " << scanLineEdgels[i].end << " angle = " << scanLineEdgels[i].end_angle << std::endl;
      stream << "  ScanLine = " << scanLineEdgels[i].ScanLineID << " run = " << scanLineEdgels[i].runID << std::endl;
      stream << "  is valid = " << (scanLineEdgels[i].valid ? "true" : "false") << std::endl;
    }
  }//end print
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
