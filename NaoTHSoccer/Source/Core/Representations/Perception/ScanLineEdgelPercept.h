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
    scanLineValidEdgels.reserve(MAX_NUMBER_OF_SCANLINE_EDGELS);
    scanLineNonValidEdgels.reserve(MAX_NUMBER_OF_SCANLINE_EDGELS);
  }

  class EndPoint
  {
  public:
    EndPoint():color(ColorClasses::none), ScanLineID(0){}
    Vector2<int> posInImage;
    Vector2<double> posOnField;
    ColorClasses::Color color;
    unsigned int ScanLineID;
  };

  unsigned int numOfSeenEdgels;
  vector<Edgel> scanLineValidEdgels;
  vector<Edgel> scanLineNonValidEdgels;

  std::vector<EndPoint> endPoints;

  void add(const Edgel& edgel)
	{
    if(edgel.valid &&  scanLineValidEdgels.size() < MAX_NUMBER_OF_SCANLINE_EDGELS)
    {
      scanLineValidEdgels.push_back(edgel);
      numOfSeenEdgels++;
    }
    else
    {
      if(!edgel.valid &&  scanLineNonValidEdgels.size() < MAX_NUMBER_OF_SCANLINE_EDGELS)
      {
        scanLineNonValidEdgels.push_back(edgel);
        numOfSeenEdgels++;
      }

    }
	}//end add

  /* reset percept */
  void reset()
  {
    numOfSeenEdgels = 0;
    endPoints.clear();
    scanLineValidEdgels.clear();
    scanLineNonValidEdgels.clear();
  }//end reset

  virtual void print(ostream& stream) const
  {
    stream << "ScanLine Edgels(" << numOfSeenEdgels << " found):" << endl << "------" << endl;
    for(unsigned int i = 0; i < scanLineValidEdgels.size(); i++)
    {
      stream << "Edgel (valid)" << i << endl;
      stream << "  Begin = " << scanLineValidEdgels[i].begin << " angle = " << scanLineValidEdgels[i].begin_angle << endl;
      stream << "  Center = " << scanLineValidEdgels[i].center << " angle = " << scanLineValidEdgels[i].center_angle << endl;
      stream << "  End = " << scanLineValidEdgels[i].end << " angle = " << scanLineValidEdgels[i].end_angle << endl;
      stream << "  ScanLine = " << scanLineValidEdgels[i].ScanLineID << " run = " << scanLineValidEdgels[i].runID << endl;
    }
    for(unsigned int i = 0; i < scanLineNonValidEdgels.size(); i++)
    {
      stream << "Edgel (invalid)" << i << endl;
      stream << "  Begin = " << scanLineNonValidEdgels[i].begin << " angle = " << scanLineNonValidEdgels[i].begin_angle << endl;
      stream << "  Center = " << scanLineNonValidEdgels[i].center << " angle = " << scanLineNonValidEdgels[i].center_angle << endl;
      stream << "  End = " << scanLineNonValidEdgels[i].end << " angle = " << scanLineNonValidEdgels[i].end_angle << endl;
      stream << "  ScanLine = " << scanLineNonValidEdgels[i].ScanLineID << " run = " << scanLineNonValidEdgels[i].runID << endl;
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


