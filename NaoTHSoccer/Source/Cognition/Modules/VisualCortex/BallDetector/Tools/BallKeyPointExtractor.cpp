/**
* @file BallKeyPointExtractor.cpp
*
* Sorted list evaluated non-overlaping patches
*/

#include "BallKeyPointExtractor.h"

using namespace naoth;
using namespace std;


BestPatchList::Patch BallKeyPointExtractor::refineKeyPoint(const BestPatchList::Patch& patch) const
{
  // todo needs a better place
  const int32_t FACTOR = getGameColorIntegralImage().FACTOR;

  Vector2i min = patch.min / FACTOR;
  Vector2i max = patch.max / FACTOR;

  // enlarge the search area
  int outer_size = max.x - min.x;
  //min.x -= outer_size/4;
  //min.y -= outer_size/4;
  //max.x += outer_size/4;
  //max.y += outer_size/4;

  int inner_size = outer_size;

  BestPatchList::Patch maxPatch;

  Vector2i point;
  // iterate different inner size
  for(inner_size = outer_size/2; inner_size + inner_size/2 < outer_size; ++inner_size) 
  {
    int border = inner_size/4;

    for(point.y = min.y+border; point.y + border + inner_size < max.y; ++point.y)
    {
      for(point.x = min.x+border; point.x + border + inner_size < max.x; ++point.x)
      {
        int size = inner_size;
        int inner = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
        double greenBelow = getGameColorIntegralImage().getDensityForRect(point.x, point.y+size, point.x+size, point.y+size+border, 1);

        if (inner*2 > size*size && greenBelow > 0.3)
        {
          int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);
          double value = (double)(inner - (outer - inner))/((double)(size+border)*(size+border));

          // scale the patch up to the image coordinates
          if(maxPatch.value == -1 || value > maxPatch.value)
          {
            maxPatch.min.x = point.x-border;
            maxPatch.min.y = point.y-border;
            maxPatch.max.x = point.x+size+border;
            maxPatch.max.y = point.y+size+border;
            maxPatch.value = value;
          }
        }
      }
    }
  }

  maxPatch.min *= FACTOR;
  maxPatch.max *= FACTOR;

  return maxPatch;
}

