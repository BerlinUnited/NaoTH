#include "BallPatchDetector.h"

using namespace naoth;
using namespace std;

BallPatchDetector::BallPatchDetector() : cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:BallPatchDetector:draw_value", "", false);
  DEBUG_REQUEST_REGISTER("Vision:BallPatchDetector:drawPatches", "draw ball key points", false);
  DEBUG_REQUEST_REGISTER("Vision:BallPatchDetector:drawPatches_px", "draw ball key points in the raw image", false);
  DEBUG_REQUEST_REGISTER("Vision:BallKeyPointExtractor:draw_value", "", false);

  getDebugParameterList().add(&params);
}

BallPatchDetector::~BallPatchDetector() 
{
  getDebugParameterList().remove(&params);
}

void BallPatchDetector::execute(const CameraInfo::CameraID id) 
{
  cameraID = id;
  getBestPatchList().clear();

  calculateKeyPointsFast(getBallDetectorIntegralImage(), getBestPatchList());

  DEBUG_REQUEST("Vision:BallPatchDetector:drawPatches",
    CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
    PEN("FF0000", 1);
    for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i) {
      BOX((*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
    }
  );

  DEBUG_REQUEST("Vision:BallPatchDetector:drawPatches_px",
    for(BestPatchList::reverse_iterator i = getBestPatchList().rbegin(); i != getBestPatchList().rend(); ++i) {
      RECT_PX(ColorClasses::red, (*i).min.x, (*i).min.y, (*i).max.x, (*i).max.y);
    }
  );
}

BestPatchList::Patch BallPatchDetector::refineKeyPoint(const BestPatchList::Patch& patch) const
{
  // todo needs a better place
  const int32_t FACTOR = getGameColorIntegralImage().FACTOR;


  Vector2<unsigned int> min = patch.min / FACTOR;
  Vector2<unsigned int> max = patch.max / FACTOR;

  // enlarge the search area
  unsigned int outer_size = max.x - min.x;
  //min.x -= outer_size/4;
  //min.y -= outer_size/4;
  //max.x += outer_size/4;
  //max.y += outer_size/4;

  BestPatchList::Patch maxPatch;

  Vector2<unsigned int> point;

  // iterate different inner size
  for(unsigned int inner_size = outer_size/2; inner_size + inner_size/2 < outer_size; ++inner_size)
  {
    unsigned int border = inner_size/4;

    for(point.y = min.y+border; point.y + border + inner_size < max.y; ++point.y)
    {
      for(point.x = min.x+border; point.x + border + inner_size < max.x; ++point.x)
      {
        unsigned int inner = getGameColorIntegralImage().getSumForRect(point.x, point.y, point.x+inner_size, point.y+inner_size, 0);
        double greenBelow = getGameColorIntegralImage().getDensityForRect(point.x, point.y+inner_size, point.x+inner_size, point.y+inner_size+border, 1);

        if (inner*2 > inner_size*inner_size && greenBelow > 0.3)
        {
          unsigned int outer = getGameColorIntegralImage().getSumForRect(point.x-border, point.y+inner_size, point.x+inner_size+border, point.y+inner_size+border, 0);
          double value = (double)(inner - (outer - inner))/((double)(inner_size+border)*(inner_size+border));

          // scale the patch up to the image coordinates
          if(maxPatch.value == -1 || value > maxPatch.value)
          {
            maxPatch.min.x = static_cast<int>(point.x-border);
            maxPatch.min.y = static_cast<int>(point.y-border);
            maxPatch.max.x = static_cast<int>(point.x+inner_size+border);
            maxPatch.max.y = static_cast<int>(point.y+inner_size+border);
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

