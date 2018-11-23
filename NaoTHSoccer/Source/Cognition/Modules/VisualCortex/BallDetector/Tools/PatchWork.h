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
#include <Tools/Debug/NaoTHAssert.h>
#include <vector>

class PatchWork
{
public:
  static void subsampling(const naoth::Image& image, std::vector<unsigned char>& result, int x0, int y0, int x1, int y1, int size);
  static void subsampling(const naoth::Image& image, const FieldColorPercept& fielColorPercept, std::vector<BallCandidates::ClassifiedPixel>& result, int x0, int y0, int x1, int y1, int size);

  static void subsampling(const naoth::Image& image, BallCandidates::Patch& target) {
    ASSERT(target.data.size() == target.size()*target.size());
    subsampling(image, target.data, target.min.x, target.min.y, target.max.x, target.max.y, static_cast<int>(target.size()));
  }

  static void subsampling(const naoth::Image& image, const FieldColorPercept& fielColorPercept, BallCandidates::PatchYUVClassified& target) {
    ASSERT(target.data.size() == target.size()*target.size());
    subsampling(image, fielColorPercept, target.data, target.min.x, target.min.y, target.max.x, target.max.y, static_cast<int>(target.size()));
  }

  static void multiplyBrightness(double factor, std::vector<unsigned char>& patch);
  static void multiplyBrightness(double factor, BallCandidates::PatchYUVClassified& patch);

  static void toPatch(const BallCandidates::PatchYUVClassified& src, BallCandidates::Patch& target);
  static BallCandidates::PatchesList toPatchList(const BallCandidates::PatchYUVClassifiedList &src);

  static double calculateContrast(const BallCandidates::PatchYUVClassified& patch);
  static double calculateContrastIterative(const BallCandidates::PatchYUVClassified& patch);
  static double calculateContrastIterative2nd(const BallCandidates::PatchYUVClassified& patch);

  static double calculateContrast(const Image& image,  const FieldColorPercept& fielColorPercept, int x0, int y0, int x1, int y1, int size);
  static double calculateContrastIterative(const Image& image,  const FieldColorPercept& fielColorPercept, int x0, int y0, int x1, int y1, int size);
  static double calculateContrastIterative2nd(const Image& image,  const FieldColorPercept& fielColorPercept, int x0, int y0, int x1, int y1, int size);
  
};

#endif // _PatchWork_H_
