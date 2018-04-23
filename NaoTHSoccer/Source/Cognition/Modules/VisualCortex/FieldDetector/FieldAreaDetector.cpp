#include "FieldAreaDetector.h"

FieldAreaDetector::FieldAreaDetector()
:
  cameraID(CameraInfo::Bottom)
{
  //DEBUG_REQUEST_REGISTER("Vision:FieldDetector:mark_field_polygon", "mark polygonal boundary of the detected field on the image", false);
  getDebugParameterList().add(&theParameters);
}


FieldAreaDetector::~FieldAreaDetector()
{}


void FieldAreaDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  if(!getBallDetectorIntegralImage().isValid())
    return;

  const int32_t factor = getBallDetectorIntegralImage().FACTOR;

  int grid_size = 32;
  double proportion_of_green = 0.5;

  uint32_t minX, maxX, minY, maxY;
  for (minX=0; minX<getBallDetectorIntegralImage().getWidth(); minX+=grid_size) {
    maxX = minX+grid_size;
    if (!(maxX < getBallDetectorIntegralImage().getWidth())) {
      maxX = getBallDetectorIntegralImage().getWidth()-1;
    }

    for(minY=getBallDetectorIntegralImage().getHeight()-1; minY > 0; minY = minY >= grid_size ? minY-grid_size : 0) {
      maxY = minY >= grid_size ? minY-grid_size : 0;

      double density = getBallDetectorIntegralImage().getDensityForRect(minX, minY, maxX, maxY, 1);
      if(density >= proportion_of_green) {
        RECT_PX(ColorClasses::green, minX*factor, minY*factor, maxX*factor, maxY*factor);
      } else {
        RECT_PX(ColorClasses::red, minX*factor, minY*factor, maxX*factor, maxY*factor);
      }
    }
  }
}
