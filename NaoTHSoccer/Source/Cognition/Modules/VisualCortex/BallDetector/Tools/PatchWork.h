/**
* @file PatchWork.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _PatchWork_H_
#define _PatchWork_H_

#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/BallCandidates.h"
#include "Representations/Perception/FieldColorPercept.h"
#include <vector>

class PatchWork
{
public:
  static void subsampling(const naoth::Image& image, std::vector<unsigned char>& result, int x0, int y0, int x1, int y1, int size);

  static void subsampling(const naoth::Image& image, const FieldColorPercept& fielColorPercept, std::vector<BallCandidates::ClassifiedPixel>& result, int x0, int y0, int x1, int y1, int size);

  static void multiplyBrightness(double factor, std::vector<unsigned char>& patch);

  static void toPatch(const BallCandidates::PatchYUVClassified& src, BallCandidates::Patch& target);
  static BallCandidates::PatchesList toPatchList(const BallCandidates::PatchYUVClassifiedList &src);
};

#endif // _PatchWork_H_
