#include "IntegralFieldDetector.h"
#include <cmath>

#include <Tools/Math/ConvexHull.h>

IntegralFieldDetector::IntegralFieldDetector()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:IntegralFieldDetector:draw_grid", "", false);
  DEBUG_REQUEST_REGISTER("Vision:IntegralFieldDetector:draw_end_cell", "", false);
  DEBUG_REQUEST_REGISTER("Vision:IntegralFieldDetector:mark_field_polygon", "", false);
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
  endpoints.clear();

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
    int horizon_height = std::max(
          (int) (getArtificialHorizon().point(cell.maxX*factor).y/factor),
          (int) (getArtificialHorizon().point(cell.minX*factor).y/factor)
        );
    horizon_height = std::max(0, horizon_height);

    bool isLastCell = false;
    int skipped = 0;
    int successive_green = 0;
    int green_found = 0;
    for(cell.maxY = height-1; !isLastCell; cell.maxY = cell.minY) {
      cell.minY = cell.maxY-grid_size;
      if(cell.minY < horizon_height) {
        cell.minY = horizon_height;
        cell.maxY = cell.minY + grid_size;
        isLastCell = true;
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
        skipped = 0;
        ++successive_green;

        if (successive_green >= params.min_successive_green) {
          last_green_cell = cell;
          green_found = true;
        }

      } else {
        ++skipped;
        if (skipped > params.max_skip_cells) {
          successive_green = 0;
        }
      }
    }

    if(green_found){
      Vector2i endpoint;
      find_endpoint(last_green_cell, endpoint);
      endpoints.push_back(endpoint);

      DEBUG_REQUEST("Vision:IntegralFieldDetector:draw_end_cell",
        RECT_PX(ColorClasses::skyblue, last_green_cell.minX*factor, last_green_cell.minY*factor+1, last_green_cell.maxX*factor, last_green_cell.maxY*factor-1);
        CIRCLE_PX(ColorClasses::orange, endpoint.x, endpoint.y, 1);
      );
    }
  }

  if(endpoints.size() > 1) {
    create_field();
  }
}

void IntegralFieldDetector::find_endpoint(const Cell& cell, Vector2i& endpoint) {
  /*
  // estimate endpoint
  int grid_size = cell.maxY - cell.minY;
  int pixels_per_cell = (grid_size+1) * (grid_size+1);

  double cut = cell.sum_of_green / (double)pixels_per_cell;
  endpoint.x = (cell.minX+cell.maxX)/2 * factor;
  endpoint.y = (cell.maxY-(int)(cut*(grid_size+1))) * factor;
  */
  double score = 0.;
  double best_score = 0.;
  int minY = cell.minY*factor;
  int width = getImage().width();
  if(cell.minX*factor < width/2) {
    endpoint.x = cell.minX * factor;
  } else {
    endpoint.x = cell.maxX * factor;
  }
  endpoint.y = cell.maxY*factor;
  for (int y=endpoint.y; y>=minY; --y) {
    Pixel pixel = getImage().get(endpoint.x, y);
    if (getFieldColorPercept().greenHSISeparator.isColor(pixel)) {
      score += params.positive_score;
    } else {
      score += params.negative_score;
    }
    if(score >= best_score) {
      best_score = score;
      endpoint.y = y;
    }
  }
}

void IntegralFieldDetector::create_field() {
  Vector2i p1 = endpoints.front();
  p1.y = getImage().height() - 1;
  Vector2i p2 = endpoints.back();
  p2.y = getImage().height() - 1;

  endpoints.push_back(p1);
  endpoints.push_back(p2);
  std::vector<Vector2i> result = ConvexHull::convexHull(endpoints);

  FieldPercept::FieldPoly fieldPoly;
  for(size_t i = 0; i < result.size(); i++)
  {
    fieldPoly.add(result[i]);
  }

  // add field to percept
  getFieldPercept().setField(fieldPoly, getArtificialHorizon());
  // check result
  getFieldPercept().valid = fieldPoly.getArea() >= 5600;

  DEBUG_REQUEST("Vision:IntegralFieldDetector:mark_field_polygon",
    int idx = 0;
    ColorClasses::Color color = getFieldPercept().valid ? ColorClasses::blue : ColorClasses::red;
    const FieldPercept::FieldPoly& poly = getFieldPercept().getValidField();
    for(int i = 1; i < poly.length; i++)
    {
      LINE_PX(color, poly[idx].x, poly[idx].y, poly[i].x, poly[i].y);
      idx = i;
    }
  );
}
