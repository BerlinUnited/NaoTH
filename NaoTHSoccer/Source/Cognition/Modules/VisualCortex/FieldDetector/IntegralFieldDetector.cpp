#include "IntegralFieldDetector.h"
#include <cmath>

#include <Tools/Math/ConvexHull.h>

IntegralFieldDetector::IntegralFieldDetector()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:IntegralFieldDetector:draw_grid", "", false);
  getDebugParameterList().add(&params);
}


IntegralFieldDetector::~IntegralFieldDetector()
{
  getDebugParameterList().remove(&params);
}

void IntegralFieldDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  if(getBodyContour().timestamp != getFrameInfo().getTime()
    || !getBallDetectorIntegralImage().isValid()) {
    return;
  }
  factor = getBallDetectorIntegralImage().FACTOR;

  int grid_size = (cameraID==CameraInfo::Top ? params.grid_size_top : params.grid_size_bottom) / factor;
  int width = getBallDetectorIntegralImage().getWidth();
  int height = getBallDetectorIntegralImage().getHeight();
  double cells_horizontal = (width-1.0) / grid_size;
  int n_cells_horizontal = (int) cells_horizontal;
  double offset = (cells_horizontal - n_cells_horizontal) * grid_size;
  offset /= (n_cells_horizontal-1);
  int pixels_per_cell = (grid_size+1)*(grid_size+1);
  int min_green = (int)(pixels_per_cell*params.proportion_of_green);

  Cell last_green_cell;
  Cell cell;
  for (double minX=0.; ((int) minX) + grid_size < width; minX = minX + grid_size + offset) {
    cell.minX = (int) (minX + .5);
    cell.maxX = cell.minX + grid_size;

    // Get first pos not occupied by body.
    // HACK: This does not take the difference in refinement to the body contour into consideration
    Vector2i start_left(cell.minX*factor, getImage().height()-1);
    Vector2i start_right(cell.maxX*factor, getImage().height()-1);
    int start_y = std::min(getBodyContour().getFirstFreeCell(start_left).y/factor,
                           getBodyContour().getFirstFreeCell(start_right).y/factor);


    int horizon_height = std::max(
          (int) (getArtificialHorizon().point(cell.maxX*factor).y/factor),
          (int) (getArtificialHorizon().point(cell.minX*factor).y/factor)
        );
    horizon_height = std::max(0, horizon_height);

    bool green_found = false;
    bool isLastCell = false;
    int skipped = 0;
    for(cell.maxY = height-1; !isLastCell; cell.maxY = cell.minY) {
      cell.minY = cell.maxY-grid_size;
      if(cell.minY < horizon_height) {
        cell.minY = horizon_height;
        cell.maxY = cell.minY + grid_size;
        isLastCell = true;
      }
      if(cell.maxY > start_y) {
        continue;
      }

      cell.sum_of_green = getBallDetectorIntegralImage().getSumForRect(cell.minX, cell.minY, cell.maxX, cell.maxY, 1);

      DEBUG_REQUEST("Vision:IntegralFieldDetector:draw_grid",
        ColorClasses::Color color;
        if(cell.sum_of_green >= min_green) {
          color = ColorClasses::green;
        } else {
          color = ColorClasses::red;
        }
        RECT_PX(color, cell.minX*factor, cell.minY*factor+1, cell.maxX*factor, cell.maxY*factor-1);
      );

      if(cell.sum_of_green >= min_green) {
        last_green_cell = cell;
        green_found = true;
        ++skipped;
      }
    }
  }
}
