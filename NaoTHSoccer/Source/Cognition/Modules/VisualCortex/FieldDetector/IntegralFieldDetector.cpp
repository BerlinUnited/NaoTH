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

  const int width = getImage().width();
  const int height = getImage().height();

  // calculate theoretical cell sizes
  // TODO: create struct for parameters (TOP/BOTTOM Camera)
  double cell_width, cell_height;
  if(cameraID==CameraInfo::Top) {
      cell_width = static_cast<double>(width) / params.column_count_top;
      cell_height = static_cast<double>(height) / params.row_count_top;
  } else {
      cell_width = static_cast<double>(width) / params.column_count_bottom;
      cell_height = static_cast<double>(height) / params.row_count_bottom;
  }
  if (cell_width < 1 || cell_height < 1) {
      // invalid parameters
      return;
  }

  bool first = true;
  bool green_found = false;
  bool former_green = false;
  Cell last_green_cell;
  Cell cell;
  // iterate columns
  // TODO warum (cell_width-1)
  for (double x=0.0; x + cell_width-1 < width; x += cell_width)
  {
    // calculate column bounds
    cell.minX = static_cast<int>(x);
    cell.maxX = static_cast<int>(x + cell_width-1);

    // calculate minimal y value under the horizon for the current column
    int horizon_height = static_cast<int>(std::max(
        getArtificialHorizon().point(cell.maxX).y,
        getArtificialHorizon().point(cell.minX).y));
    int min_scan_y = Math::clamp(horizon_height, 0, height-1);

    int skipped = 0;
    int successive_green = 0;
    int cell_number = 1;

    // iterate rows; scan from bottom to top
    for(double y = height-1; y - (cell_height-1) >= min_scan_y; y -= cell_height)
    {
      // TODO warum (cell_height-1)
      // calculate row bounds
      cell.maxY = static_cast<int>(y);
      cell.minY = static_cast<int>(y - (cell_height-1));

      // check if the cell is mostly green
      const double cell_green_density = cell.calc_green_density(getBallDetectorIntegralImage());
      const bool cell_is_green = cell_green_density >= params.proportion_of_green;

      DEBUG_REQUEST("Vision:IntegralFieldDetector:draw_grid",
        IMAGE_DRAWING_CONTEXT;
        CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));

        std::string color;
        if(cell_is_green) {
          color = "00FF00";
        } else {
          color = "FF00FF";
        }
        PEN(color, 1);
        BOX(cell.minX, cell.minY, cell.maxX, cell.maxY);
      );

      if(cell_is_green) {
        skipped = 0;
        ++successive_green;
        // TODO: comment
        if (successive_green >= params.min_successive_green || cell_number == successive_green) {
          last_green_cell = cell;
          former_green = true;
          green_found = true;
        }
      } else {
        // check if cell has enough green where it might contain the field border
        // TODO: former_green -> last_cell_was_green
        if(former_green && cell_green_density >= params.end_proportion_of_green) {
          last_green_cell = cell;
        }
        ++skipped;
        if (skipped > params.max_skip_cells) {
          successive_green = 0;
        }
        former_green = false;
      }
      cell_number++;
    }

    // determing the endpoint
    if(green_found){
      // Additionally Left most cell: Determine the left most endpoint so that there is no gap
      if(first) {
        Vector2i left_endpoint;
        find_endpoint(last_green_cell.minX, last_green_cell, left_endpoint);
        endpoints.push_back(left_endpoint);
        first = false;
      }
      Vector2i endpoint;
      find_endpoint((last_green_cell.minX + last_green_cell.maxX) / 2,
                    last_green_cell, endpoint);
      endpoints.push_back(endpoint);
    }
  } // end scan

  // Right most cell: Determine the right most endpoint so that there is no gap
  if(green_found) {
    Vector2i right_endpoint;
    find_endpoint(last_green_cell.maxX, last_green_cell, right_endpoint);
    endpoints.push_back(right_endpoint);
  }

  if(endpoints.size() > 1) {
    create_field();
  }
}

void IntegralFieldDetector::find_endpoint(int x, const Cell& cell, Vector2i& endpoint) {
  double score = 0.;
  double best_score = 0.;
  endpoint.x = x;
  endpoint.y = cell.maxY;
  for (int y=endpoint.y; y>=cell.minY; --y) {
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
    BOX(cell.minX, cell.minY, cell.maxX, cell.maxY);
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
  // TODO: field should be valid if no field is detected (add field is_empty)
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
