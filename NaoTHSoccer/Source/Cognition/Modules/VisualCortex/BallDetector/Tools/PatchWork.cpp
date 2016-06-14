/**
* @file PatchWork.cpp
*
* Sorted list evaluated non-overlaping patches
*/

#include "PatchWork.h"

using namespace naoth;

void PatchWork::subsampling(const Image& image, std::vector<unsigned char>& result, int x0, int y0, int x1, int y1) 
{
  x0 = std::max(0, x0);
  y0 = std::max(0, y0);
  x1 = std::min((int)image.width()-1, x1);
  y1 = std::min((int)image.height()-1, y1);

  // TODO: magic number
  const double size = 12.0;
  result.resize((int)(size*size));

  double width_step = static_cast<double>(x1 - x0) / size;
  double height_step = static_cast<double>(y1 - y0) / size;

  int xi = 0;
  
  for(double x = x0 + width_step/2.0; x < x1; x += width_step) 
  {
    int yi = 0;
    for(double y = y0 + height_step/2.0; y < y1; y += height_step) 
    {
      unsigned char yy = image.getY((int)(x + 0.5), (int)(y + 0.5));
      result[xi*12 + yi] = yy;
      yi++;
    }
    xi++;
  }
}
