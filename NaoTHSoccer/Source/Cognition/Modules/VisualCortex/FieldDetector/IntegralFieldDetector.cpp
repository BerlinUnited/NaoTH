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
  getFieldPercept().reset();
  endpoints.clear();

  bool set_image_as_field = (cameraID==CameraInfo::Top)? params.set_whole_image_as_field_top:
                                                         params.set_whole_image_as_field_bottom;
  if(set_image_as_field) {
    // skip field detection and set whole image under the horizon as field
    // set endpoints to the upper image corners
    endpoints = {{0, 0}, {static_cast<int>(getImage().width())-1, 0}};
    create_field();
    return;
  }

  // TODO: maybe rename getBallDetectorIntegralImage to IntegralImage
  // TODO: isValid necessary?
  if(!getBallDetectorIntegralImage().isValid()) {
    return;
  }

  // NOTE: the factor may be different for top and bottom
  factor = getBallDetectorIntegralImage().FACTOR;
  const int width = getBallDetectorIntegralImage().getWidth();
  const int height = getBallDetectorIntegralImage().getHeight();

  const int cell_size_px = cameraID==CameraInfo::Top ? params.cell_size_px_top:
                                                       params.cell_size_px_bottom;
  // cell size in the integral image
  const int cell_size = cell_size_px / factor;

  // determine the number of horizontal and vertical cells
  const int n_cells_horizontal = width / cell_size;
  const int n_cells_vertical = height / cell_size;

  // pixels lost due to integer division
  // TODO: Rest is expected to be less then n_cells
  int rest_H = width - (n_cells_horizontal * cell_size);
  int rest_V = height - (n_cells_vertical * cell_size);

  const int pixels_per_cell = cell_size * cell_size;
  const int min_green = (int)(pixels_per_cell*params.proportion_of_green);
  const int min_end_green = (int)(pixels_per_cell*params.end_proportion_of_green);

  bool first = true;
  bool green_found = false;
  bool former_green = false;
  Cell last_green_cell;
  Cell cell;
  // TODO: simplify calculations of cell bounds.
  // In perticular get rid of bilataral usage of cell.maxX and cell.minY
  // where the integer devision rest is added.
  // In short: Make everything double and calculate grid sizes and thresholds individually
  for (cell.minX=0; cell.minX + cell_size-1 < width; cell.minX = cell.maxX + 1)
  {
    // FIXME: check whether the -1 is correct here with the use of integral image
    cell.maxX = cell.minX + cell_size-1;
    int horizon_height = std::max(
          (int) (getArtificialHorizon().point(toImage(cell.maxX)).y),
          (int) (getArtificialHorizon().point(toImage(cell.minX)).y)
        );
    int min_scan_y = Math::clamp(toIntegral(horizon_height), 0, height-1);

    int skipped = 0;
    int successive_green = 0;
    int rest = rest_V; // copy, because we need it several times
    int cell_number = 1;
    // scan up
    for(cell.maxY = height-1; cell.maxY - cell_size + 1 >= min_scan_y; cell.maxY = cell.minY - 1)
    {
      // FIXME: check whether the -1 is correct here with the use of integral image
      cell.minY = cell.maxY - cell_size + 1;

      // calculate number of green pixels in the cell
      cell.sum_of_green = getBallDetectorIntegralImage().getSumForRect(
            cell.minX, cell.minY, cell.maxX, cell.maxY, 1);

      DEBUG_REQUEST("Vision:IntegralFieldDetector:draw_grid",
        IMAGE_DRAWING_CONTEXT;
        CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));

        std::string color;
        if(cell.sum_of_green >= min_green) {
          color = "00FF00";
        } else {
          color = "FF00FF";
        }
        PEN(color, 1);
        BOX(toImage(cell.minX), toImage(cell.minY),
            toImage(cell.maxX), toImage(cell.maxY));
      );

      if(cell.sum_of_green >= min_green) {
        skipped = 0;
        ++successive_green;

        if (successive_green >= params.min_successive_green
            || cell_number == successive_green) {
          last_green_cell = cell;
          former_green = true;
          green_found = true;
        }
      } else {
        if(former_green && cell.sum_of_green > min_end_green) {
          last_green_cell = cell;
        }
        ++skipped;
        if (skipped > params.max_skip_cells) {
          successive_green = 0;
        }
        former_green = false;
      }
      // ensure cells will end on the upper image border
      if(rest > 0) {
        rest--;
        cell.minY--;
      }
      cell_number++;
    }

    if(green_found){
      if(first) {
        // Left most cell: Determine the left most endpoint so that there is no gap
        Vector2i left_endpoint;
        find_endpoint(toImage(last_green_cell.minX), last_green_cell, left_endpoint);
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

  if(green_found) {
    // Right most cell: Determine the right most endpoint so that there is no gap
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
    IMAGE_DRAWING_CONTEXT;
    CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));

    PEN("1CFFE4", 1);
    BOX(toImage(cell.minX), toImage(cell.minY),
        toImage(cell.maxX), toImage(cell.maxY));
    PEN("FFFF00", 1);
    CIRCLE(endpoint.x, endpoint.y, 2);
  );
}

void IntegralFieldDetector::create_field() {
  // Create projection if the first and last endpoint on the lower image border
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

  // add field to percept; ensures field polygon is under the horizon (required if set_whole_image_as_field is set)
  getFieldPercept().setField(fieldPoly, getArtificialHorizon());
  // check result
  getFieldPercept().valid = fieldPoly.getArea() >= params.min_field_area;

  DEBUG_REQUEST("Vision:IntegralFieldDetector:mark_field_polygon",
    IMAGE_DRAWING_CONTEXT;
    CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));

    std::string color = getFieldPercept().valid ? "FFFF00" : "FF0000";
    PEN(color, 2);

    const FieldPercept::FieldPoly& poly = getFieldPercept().getValidField();
    for(size_t i = 1; i < poly.size(); i++)
    {
      LINE(poly[i-1].x, poly[i-1].y, poly[i].x, poly[i].y);
    }
  );
}
