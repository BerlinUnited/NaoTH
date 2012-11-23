/** 
 * @file ScanLineEdgelPercept.h
 *
 * Declaration of class ScanLineEdgelPercept
 */

#ifndef __ScanLineEdgelPercept_h_
#define __ScanLineEdgelPercept_h_

#include "Tools/Math/Vector2.h"
#include "Tools/ImageProcessing/LineDetectorConstParameters.h"
#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/ColorClasses.h"

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <vector>

class ScanLineEdgelPercept : public naoth::Printable
{ 
public:
  ScanLineEdgelPercept()
    :
    numOfSeenEdgels(0)
  {
  }

  class EndPoint
  {
  public:
    EndPoint():color(ColorClasses::none), ScanLineID(0), valid(false){}
    Vector2<int> posInImage;
    Vector2<double> posOnField;
    ColorClasses::Color color;
    unsigned int ScanLineID;
    bool valid;
  };

  /** */
  unsigned int numOfSeenEdgels;
  /** */
  Edgel scanLineEdgels[MAX_NUMBER_OF_SCANLINE_EDGELS];

  /** */
  std::vector<EndPoint> endPoints;


  void add(const Edgel& edgel)
  {
    if(numOfSeenEdgels < MAX_NUMBER_OF_SCANLINE_EDGELS)
    {
      scanLineEdgels[numOfSeenEdgels] = edgel;
      numOfSeenEdgels++;
    }//end if
  }//end add

  /* reset percept */
  void reset()
  {
    numOfSeenEdgels = 0;
    endPoints.clear();
  }//end reset

  virtual void print(std::ostream& stream) const
  {
    stream << "ScanLine Edgels:" << std::endl << "------" << std::endl;
    for(unsigned int i = 0; i < numOfSeenEdgels; i++)
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

namespace naoth
{
  template<>
  class Serializer<ScanLineEdgelPercept>
  {
  public:
    static void serialize(const ScanLineEdgelPercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, ScanLineEdgelPercept& representation);
  };
}

#endif //__ScanLineEdgelPercept_h_
