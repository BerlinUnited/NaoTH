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

class LineGraphPercept
{ 
public:
  std::vector<Edgel> edgels;
};

#endif // _LineGraphPercept_h_


