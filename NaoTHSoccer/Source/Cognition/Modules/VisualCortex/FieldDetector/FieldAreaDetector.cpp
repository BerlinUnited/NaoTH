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

  // TODO Parameter
  int32_t grid_size = 32/factor;
  double green_weight = .5;

  const double pixels_per_grid = static_cast<double>(grid_size*grid_size);
  const double green_pixels = pixels_per_grid*params.proportion_of_green;

  int32_t minX, maxX, minY, maxY;

  double score = 0, best_score = -1;
  int32_t best_grid_x=0, best_grid_y=0;

  // TODO start after own body

  for (minX = 0; minX < (int)getBallDetectorIntegralImage().getWidth(); minX += grid_size) {
    maxX = std::min(minX + grid_size, (int)getBallDetectorIntegralImage().getWidth()-1);

    for(minY = getBallDetectorIntegralImage().getHeight()-grid_size; minY >= 0; minY -= grid_size) {
      maxY = std::min(minY + grid_size, (int)getBallDetectorIntegralImage().getHeight()-1);

      double sum_of_green = (int)getBallDetectorIntegralImage().getSumForRect(minX, minY, maxX, maxY, 1);

      if(sum_of_green >= green_pixels) {
        RECT_PX(ColorClasses::green, minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);
      } else {
        RECT_PX(ColorClasses::red, minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);
      }
      double t = sum_of_green/static_cast<double>(grid_size*grid_size);
      Color color = green*t + white*(1-t);
      PEN(color, 0.1);
      FILLBOX(minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);

      score += (green_weight*sum_of_green) - ((1-green_weight)*(pixels_per_grid - sum_of_green));
      std::cout << "score " << score << std::endl;
      if(score >= best_score) {
        best_score = score;
        best_grid_x = minX;
        best_grid_y = minY;
      }
    }
    std::cout << "best_score: " << best_score << std::endl;
    if(best_score > 0) {
      RECT_PX(ColorClasses::blue, best_grid_x*factor+1, best_grid_y*factor+1,
        (best_grid_x+grid_size)*factor-1, (best_grid_y-grid_size)*factor-1);
    }
    score = 0;
    best_score = -1;
  }


}
