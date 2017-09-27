/**
* @file BlackSpotExtractor.cpp
*
* Sorted list evaluated non-overlaping patches
*/

#include "BlackSpotExtractor.h"

using namespace naoth;

void BlackSpotExtractor::calculateKeyPoints(const GameColorIntegralImage& image, BestPatchList& bestBlack, int minX, int minY, int maxX, int maxY)
{
  bestBlack.clear();

  // todo needs a better place
  // needed to convert between image coordinates and intergral image coordinates
  const int32_t FACTOR = image.FACTOR;

  Vector2i center;
  Vector2i point;
  
  for(point.y = minY/FACTOR; point.y < maxY/FACTOR; ++point.y)
  {
    // HACK: assume square
    int size   = ((maxX - minX) / 5)/FACTOR; // integral coords
    int border = size / 2;

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border+1 >= (int)image.getHeight()) {
      continue;
    }
    
    for(point.x = minX/FACTOR + border; point.x + size + border < maxX/FACTOR; ++point.x)
    {
      int innerBlack = image.getSumForRect(point.x, point.y, point.x+size, point.y+size, 2);
      int innerWhite = image.getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      int innerGreen = image.getSumForRect(point.x, point.y, point.x+size, point.y+size, 1);

      // number of non-white pixels
      int innerDark = ((size+1)*(size+1) - innerWhite - innerGreen);

      // at least 33% should be non-white and at least one black
      if (innerDark*3 > (size+1)*(size+1) && innerBlack > 0)
      {
        int outerWhite = image.getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);

        double value = (double)(innerDark + outerWhite)/((double)(size+border)*(size+border));

        bestBlack.add( 
          (point.x-border)*FACTOR, 
          (point.y-border)*FACTOR, 
          (point.x+size+border)*FACTOR, 
          (point.y+size+border)*FACTOR, 
          value);
      }
    }
  }
}

int BlackSpotExtractor::calculateKeyPointsBlackBetter(const BallDetectorIntegralImage& image, BestPatchList& bestBlack, int minX, int minY, int maxX, int maxY)
{
  bestBlack.clear();

  // todo needs a better place
  // needed to convert between image coordinates and intergral image coordinates
  const int32_t FACTOR = image.FACTOR;

  Vector2i center;
  Vector2i point;
  
  for(point.y = minY/FACTOR; point.y < maxY/FACTOR; ++point.y)
  {
    // HACK: assume square
    // HACK: assume square
    int size   = ((maxX - minX) / 5)/FACTOR; // integral coords
    int border = size / 2;

    // smalest ball size == 3 => ball size == FACTOR*3 == 12
    if (point.y <= border || point.y+size+border+1 >= (int)image.getHeight()) {
      continue;
    }
    
    for(point.x = minX/FACTOR + border; point.x + size + border < maxX/FACTOR; ++point.x)
    {
      int inner = image.getSumForRect(point.x, point.y, point.x+size, point.y+size, 0);
      int outer = image.getSumForRect(point.x-border, point.y+size, point.x+size+border, point.y+size+border, 0);

      int innerGreen = image.getSumForRect(point.x, point.y, point.x+size, point.y+size, 1);

      if(innerGreen == 0) {
        double value = (double)(outer - inner)/((double)(size+border)*(size+border));

        bestBlack.add( 
            (point.x-border)*FACTOR, 
            (point.y-border)*FACTOR, 
            (point.x+size+border)*FACTOR, 
            (point.y+size+border)*FACTOR, 
            value);
      }
    }
  }

  return static_cast<int>(bestBlack.size());
}
