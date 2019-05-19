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
  if(!getBallDetectorIntegralImage().isValid()) {
    return;
  }
  endpoints.clear();

  factor = getBallDetectorIntegralImage().FACTOR;
  int width = getBallDetectorIntegralImage().getWidth();
  int height = getBallDetectorIntegralImage().getHeight();

  int grid_size = (cameraID==CameraInfo::Top ? params.grid_size_top:
                                               params.grid_size_bottom) / factor;

  // determine number of grids
  int n_cells_horizontal = width / grid_size;
  int n_cells_vertical = height / grid_size;

  // pixels lost due to integer division
  int rest_H = width - (n_cells_horizontal * grid_size);
  int rest_V = height - (n_cells_vertical * grid_size);

  int pixels_per_cell = (grid_size+1)*(grid_size+1);
  int min_green = (int)(pixels_per_cell*params.proportion_of_green);

  bool first = true;
  bool green_found = false;
  Cell last_green_cell;
  Cell cell;
  for (cell.minX=0; cell.minX + grid_size-1 < width; cell.minX = cell.maxX + 1) {
    cell.maxX = cell.minX + grid_size-1;
    int horizon_height = std::max(
          (int) (getArtificialHorizon().point(toImage(cell.maxX)).y),
          (int) (getArtificialHorizon().point(toImage(cell.minX)).y)
        );
    int min_scan_y = Math::clamp(toIntegral(horizon_height), 0, height-1);

    int skipped = 0;
    int successive_green = 0;
    int rest = rest_V;
    // scan up
    for(cell.maxY = height-1; cell.maxY - grid_size + 1 >= min_scan_y; cell.maxY = cell.minY - 1) {
      cell.minY = cell.maxY - grid_size + 1;
      cell.sum_of_green = getBallDetectorIntegralImage().getSumForRect(
            cell.minX, cell.minY, cell.maxX, cell.maxY, 1);

      DEBUG_REQUEST("Vision:IntegralFieldDetector:draw_grid",
        ColorClasses::Color color;
        if(cell.sum_of_green >= min_green) {
          color = ColorClasses::green;
        } else {
          color = ColorClasses::red;
        }
        RECT_PX(color, toImage(cell.minX), toImage(cell.minY),
                       toImage(cell.maxX), toImage(cell.maxY));
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
      // ensure cells will end on the upper image border
      if(rest > 0) {
        rest--;
        cell.minY--;
      }
    }

    if(green_found){
      if(first) { // find the left most endpoint
        Vector2i left_endpoint;
        find_endpoint(toImage(last_green_cell.minX),
                      last_green_cell, left_endpoint);
        endpoints.push_back(left_endpoint);
        first = false;
      }
      Vector2i endpoint;
      find_endpoint(toImage((last_green_cell.minX + last_green_cell.maxX) / 2),
                    last_green_cell, endpoint);
      endpoints.push_back(endpoint);
    }

    // ensure cells will end on the right image border
    if(rest_H > 0) {
      rest_H--;
      cell.maxX++;
    }
  }

  if(green_found) { // find the right most endpoint
    Vector2i right_endpoint;
    find_endpoint((last_green_cell.maxX + 1) * factor - 1,
                  last_green_cell, right_endpoint);
    endpoints.push_back(right_endpoint);
  }

  if(endpoints.size() > 1) {
    create_field();
  }
}

void IntegralFieldDetector::find_endpoint(int x, const Cell& cell, Vector2i& endpoint) {
  double score = 0.;
  double best_score = 0.;
  int minY = toImage(cell.minY);
  endpoint.x = x;
  endpoint.y = toImage(cell.maxY);
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
  DEBUG_REQUEST("Vision:IntegralFieldDetector:draw_end_cell",
    RECT_PX(ColorClasses::skyblue, toImage(cell.minX), toImage(cell.minY),
                                   toImage(cell.maxX), toImage(cell.maxY));
    CIRCLE_PX(ColorClasses::orange, endpoint.x, endpoint.y, 1);
  );
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
    size_t idx = 0;
    ColorClasses::Color color = getFieldPercept().valid ? ColorClasses::blue : ColorClasses::red;
    const FieldPercept::FieldPoly& poly = getFieldPercept().getValidField();
    for(size_t i = 1; i < poly.size(); i++)
    {
      LINE_PX(color, poly[idx].x, poly[idx].y, poly[i].x, poly[i].y);
      idx = i;
    }
  );
}
