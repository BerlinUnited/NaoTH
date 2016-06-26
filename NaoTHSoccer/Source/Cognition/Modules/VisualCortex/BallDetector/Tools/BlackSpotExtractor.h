/**
* @file BlackSpotExtractor.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _BlackSpotExtractor_H_
#define _BlackSpotExtractor_H_

#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "BestPatchList.h"

class BlackSpotExtractor
{
public:

  // scan inside a given region in the image for black keypoints
  static void calculateKeyPoints(const MultiChannelIntegralImage& image, BestPatchList& bestBlack, int minX, int minY, int maxX, int maxY);
};

#endif // _BlackSpotExtractor_H_
