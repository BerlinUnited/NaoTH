/**
* @file PatchWork.cpp
*
* Sorted list evaluated non-overlaping patches
*/

#include "PatchWork.h"
#include <Tools/ColorClasses.h>

using namespace naoth;

void PatchWork::subsampling(const Image& image, std::vector<unsigned char>& result, int x0, int y0, int x1, int y1, int size) 
{
  x0 = std::max(0, x0);
  y0 = std::max(0, y0);
  x1 = std::min((int)image.width()-1, x1);
  y1 = std::min((int)image.height()-1, y1);

  // TODO: magic number
  //const double size = 12.0;
  result.resize(size*size);

  double width_step = static_cast<double>(x1 - x0) / static_cast<double>(size);
  double height_step = static_cast<double>(y1 - y0) / static_cast<double>(size);

  int xi = 0;
  
  for(double x = x0 + width_step/2.0; x < x1; x += width_step) 
  {
    int yi = 0;
    for(double y = y0 + height_step/2.0; y < y1; y += height_step) 
    {
      unsigned char yy = image.getY((int)(x + 0.5), (int)(y + 0.5));
      result[xi*size + yi] = yy;
      yi++;
    }
    xi++;
  }
}

void PatchWork::subsampling(const Image& image, const FieldColorPercept& fielColorPercept, std::vector<BallCandidates::ClassifiedPixel>& result, int x0, int y0, int x1, int y1, int size) 
{
  x0 = std::max(0, x0);
  y0 = std::max(0, y0);
  x1 = std::min((int)image.width()-1, x1);
  y1 = std::min((int)image.height()-1, y1);

  // TODO: magic number
  //const double size = 12.0;
  result.resize(size*size);

  double width_step = static_cast<double>(x1 - x0) / static_cast<double>(size);
  double height_step = static_cast<double>(y1 - y0) / static_cast<double>(size);

  int xi = 0;
  
  for(double x = x0 + width_step/2.0; x < x1; x += width_step) 
  {
    int yi = 0;
    for(double y = y0 + height_step/2.0; y < y1; y += height_step) 
    {
      BallCandidates::ClassifiedPixel& p = result[xi*size + yi];
      image.get((int)(x + 0.5), (int)(y + 0.5), p.pixel);

      if(fielColorPercept.greenHSISeparator.noColor(p.pixel)) {
        p.c = (unsigned char)ColorClasses::white;
      } else if(fielColorPercept.greenHSISeparator.isChroma(p.pixel)) {
        p.c = (unsigned char)ColorClasses::green;
      } else {
        p.c = (unsigned char)ColorClasses::none;
      }

      yi++;
    }
    xi++;
  }
}

void PatchWork::multiplyBrightness(double factor, std::vector<unsigned char>& patch) {
  if(factor == 1.0) {
    // nothing to do
    return;
  }
  for(unsigned int i = 0; i < patch.size(); i++) {
    double originalY = patch[i];
    double multipliedY = originalY * factor;
    double clippedY = Math::clamp(multipliedY, 0.0, 255.0);
    patch[i] = (unsigned char) Math::round(clippedY);
  }
}

void PatchWork::toPatch(const BallCandidates::PatchYUVClassified &src, BallCandidates::Patch &target)
{
  target.data.clear();;
  target.min = src.min;
  target.max = src.max;
  for(std::vector<BallCandidates::ClassifiedPixel>::const_iterator it=src.data.begin();
      it != src.data.end(); it++)
  {
    const BallCandidates::ClassifiedPixel& origPixel = *it;
    target.data.push_back(origPixel.pixel.y);
  }
}

BallCandidates::PatchesList PatchWork::toPatchList(const BallCandidates::PatchYUVClassifiedList &src)
{
  std::list<BallCandidates::Patch> result;
  for(BallCandidates::PatchYUVClassifiedList::const_iterator it=src.begin();
      it != src.end(); it++)
  {
    BallCandidates::Patch p;
    toPatch(*it, p);
    result.push_back(p);
  }
  return result;
}
