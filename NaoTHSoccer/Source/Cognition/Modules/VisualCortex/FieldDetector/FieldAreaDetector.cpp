#include "FieldAreaDetector.h"
#include <cmath>

FieldAreaDetector::FieldAreaDetector()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:FieldAreaDetector:draw_grid", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldAreaDetector:draw_best_squares", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldAreaDetector:draw_split", "", false);
  getDebugParameterList().add(&params);
}


FieldAreaDetector::~FieldAreaDetector()
{
  getDebugParameterList().remove(&params);
}


void FieldAreaDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;

  if(!getBallDetectorIntegralImage().isValid()) {
    return;
  }

  factor = getBallDetectorIntegralImage().FACTOR;
  grid_size = params.grid_size/factor;
  int n_cells_x = getBallDetectorIntegralImage().getWidth()/grid_size;
  int offset = (getBallDetectorIntegralImage().getWidth()-(n_cells_x*grid_size))/n_cells_x;
  pixels_per_cell = ((grid_size+1)*(grid_size+1));
  min_green = (int)(pixels_per_cell*params.proportion_of_green);

  int32_t minX, maxX, minY, maxY;
  int32_t last_green_grid_maxY = 0;
  int32_t last_green_grid_minY = 0;
  for (minX = offset; minX < (int)getBallDetectorIntegralImage().getWidth(); minX += grid_size+offset) {
    maxX = std::min(minX + grid_size, (int)getBallDetectorIntegralImage().getWidth()-1);
    // Hack
    Vector2i start_left(minX*factor, getImage().height()-1);
    Vector2i start_right(maxX*factor, getImage().height()-1);
    int start_y = std::min(getBodyContour().getFirstFreeCell(start_left).y/factor,
                           getBodyContour().getFirstFreeCell(start_right).y/factor);
    bool green_found = false;
    for(minY = getBallDetectorIntegralImage().getHeight()-grid_size; minY >= 0; minY -= grid_size) {
      maxY = std::min((int)minY + grid_size, (int)getBallDetectorIntegralImage().getHeight()-1);
      if(maxY > start_y) {
        continue;
      }

      int sum_of_green = getBallDetectorIntegralImage().getSumForRect(minX, minY, maxX, maxY, 1);
      if(sum_of_green >= min_green) {
        last_green_grid_minY = minY;
        last_green_grid_maxY = maxY;
        green_found = true;
      } else {
        break;
      }

      DEBUG_REQUEST("Vision:FieldAreaDetector:draw_grid",
        if(sum_of_green >= min_green) {
          RECT_PX(ColorClasses::green, minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);
        } else {
          RECT_PX(ColorClasses::red, minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);
        }
        CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        Color white(1.0,1.0,1.0,0.0); // transparent
        Color green(0.0,1.0,0.0,1.0);
        double t = sum_of_green/static_cast<double>(grid_size*grid_size);
        Color color = green*t + white*(1-t);
        PEN(color, 0.1);
        FILLBOX(minX*factor+1, minY*factor+1, maxX*factor-1, maxY*factor-1);
      );
    }

    if(green_found) {
      Cell green_cell(minX, last_green_grid_minY, maxX, last_green_grid_maxY);
      refine_cell(green_cell);

      Cell upper, lower;
      int half_grid_size = split_cell(green_cell, upper, lower);

      Vector2i endpoint;
      if (upper.sum_of_green >= params.proportion_of_green * half_grid_size * half_grid_size) {
        find_endpoint(upper, endpoint);
      } else {
        find_endpoint(lower, endpoint);
      }
    }
  }
}

void FieldAreaDetector::find_endpoint(const Cell& cell, Vector2i& endpoint) {
  int y_grid_size = cell.maxY - cell.minY;
  int pixels_per_cell = y_grid_size * y_grid_size;

  double cut = cell.sum_of_green / (double)pixels_per_cell;
  endpoint.x = ((cell.minX+cell.maxX)/2) * factor;
  endpoint.y = (cell.maxY-((int)(cut*y_grid_size))) * factor;

  DEBUG_REQUEST("Vision:FieldAreaDetector:draw_best_squares",
    RECT_PX(ColorClasses::blue, cell.minX*factor+2, cell.minY*factor+2,
      cell.maxX*factor-2, cell.maxY*factor-2);
    RECT_PX(ColorClasses::orange, endpoint.x-1, endpoint.y-1, endpoint.x+1, endpoint.y+1);
  );
}

int32_t FieldAreaDetector::split_cell(const Cell& cell, Cell& upper, Cell& lower) {
  int32_t half_grid_size = grid_size/2;

  upper.minX = lower.minX = cell.minX + (half_grid_size/2);
  upper.maxX = lower.maxX = lower.minX + half_grid_size;

  lower.minY = cell.maxY - half_grid_size;
  lower.maxY = cell.maxY;
  lower.sum_of_green = getBallDetectorIntegralImage().getSumForRect(lower.minX, lower.minY, lower.maxX, lower.maxY, 1);

  upper.maxY = lower.minY;
  upper.minY = lower.maxY - grid_size;
  upper.sum_of_green = getBallDetectorIntegralImage().getSumForRect(upper.minX, upper.minY, upper.maxX, upper.maxY, 1);

  DEBUG_REQUEST("Vision:FieldAreaDetector:draw_split",
    ColorClasses::Color color;
    if(upper.sum_of_green >= (half_grid_size*half_grid_size*params.proportion_of_green)) {
      color = ColorClasses::green;
    } else {
      color = ColorClasses::red;
    }
    RECT_PX(color, upper.minX*factor+1, upper.minY*factor+1, upper.maxX*factor-1, upper.maxY*factor-1);
    if(lower.sum_of_green >= (half_grid_size*half_grid_size*params.proportion_of_green)) {
      color = ColorClasses::green;
    } else {
      color = ColorClasses::red;
    }
    RECT_PX(color, lower.minX*factor+1, lower.minY*factor+1, lower.maxX*factor-1, lower.maxY*factor-1);
  );

  return half_grid_size;
}

void FieldAreaDetector::refine_cell(Cell& cell) {
  int sum_of_green = 0;
  // move cell up
  for(int offset = 0; offset < grid_size && cell.minY-offset>=0; ++offset) {
    sum_of_green = getBallDetectorIntegralImage().getSumForRect(cell.minX, cell.minY-offset, cell.maxX, cell.maxY-offset, 1);
    if(sum_of_green >= min_green) {
      cell.minY = cell.minY-offset;
      cell.maxY = cell.maxY-offset;
      cell.sum_of_green = sum_of_green;
    } else {
      break;
    }
  }
}
