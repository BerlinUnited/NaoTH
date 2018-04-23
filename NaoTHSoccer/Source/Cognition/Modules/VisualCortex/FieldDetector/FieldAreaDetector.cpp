#include "FieldAreaDetector.h"
#include <cmath>

FieldAreaDetector::FieldAreaDetector()
:
  cameraID(CameraInfo::Bottom)
{
  //DEBUG_REQUEST_REGISTER("Vision:FieldDetector:mark_field_polygon", "mark polygonal boundary of the detected field on the image", false);
  getDebugParameterList().add(&params);
}


FieldAreaDetector::~FieldAreaDetector()
{
  getDebugParameterList().remove(&params);
}


void FieldAreaDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  Color white(1.0,1.0,1.0,0.0); // transparent
  Color green(0.0,1.0,0.0,1.0);

  if(!getBallDetectorIntegralImage().isValid()) {
    return;
  }

  const int32_t factor = getBallDetectorIntegralImage().FACTOR;

  int32_t grid_size = 32/factor;
  const double green_pixels = static_cast<double>(grid_size*grid_size)*params.proportion_of_green;

  int32_t minX, maxX, minY, maxY;
  for (minX = 0; minX < (int)getBallDetectorIntegralImage().getWidth(); minX += grid_size) {
    maxX = std::min(minX + grid_size, (int)getBallDetectorIntegralImage().getWidth()-1);

    for(minY = getBallDetectorIntegralImage().getHeight()-grid_size; minY >= 0; minY -= grid_size) {
      maxY = std::min(minY + grid_size, (int)getBallDetectorIntegralImage().getHeight()-1);

      double density = (int)getBallDetectorIntegralImage().getSumForRect(minX, minY, maxX, maxY, 1);
      if(density >= green_pixels) {
        RECT_PX(ColorClasses::green, minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);
      } else {
        RECT_PX(ColorClasses::red, minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);
      }

      double t = density/static_cast<double>(grid_size*grid_size);
      Color color = green*t + white*(1-t);
      PEN(color, 0.1);
      FILLBOX(minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);
    }
  }
}
