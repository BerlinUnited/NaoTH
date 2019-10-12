/**
* @file LineGraphPercept.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class ProbabilisticQuadCompas
*/

#ifndef _LineGraphPercept_h_
#define _LineGraphPercept_h_

#include <vector>

#include "Tools/Math/Vector2.h"
#include "Tools/ImageProcessing/Edgel.h"
#include <Tools/DataStructures/Serializer.h>

class LineGraphPercept
{ 
public:
  void reset() {
    edgelsOnField.clear();

    edgelsInImage.clear();
    edgelsInImageTop.clear();

    lineGraphs.clear();
    lineGraphsTop.clear();
  }

public:
  //TODO: should this be double?
  std::vector<Edgel>  edgelsOnField;

  std::vector<EdgelD> edgelsInImage;
  std::vector<EdgelD> edgelsInImageTop;

  std::vector<std::vector<EdgelD>> lineGraphs;
  std::vector<std::vector<EdgelD>> lineGraphsTop;
};

namespace naoth
{
template<>
class Serializer<LineGraphPercept>
{
public:
	static void serialize(const LineGraphPercept& representation, std::ostream& stream);
	static void deserialize(std::istream& stream, LineGraphPercept& representation);
};
}

#endif // _LineGraphPercept_h_


