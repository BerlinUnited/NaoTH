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

  // Get the camera parameters of the camera image we run the detection on
  Parameters::cam_params cam_params = cameraID==CameraInfo::Top? params.top:
                                                                 params.bottom;
  if(cam_params.set_image_as_field) {
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
  const double cell_width = static_cast<double>(width) / cam_params.column_count;
  const double cell_height = static_cast<double>(height) / cam_params.row_count;

  if (std::min(cell_width, cell_height) < 1) {
      // cells are to small -> invalid parameters
      return;
  }

  bool is_first_column = true; // first column where field cells have been found
  bool green_found = false;
  bool prev_was_green = false;
  Cell last_green_cell;

  // iterate columns
  Cell cell;
  for (double x=0.0; x + cell_width-1 < width; x += cell_width)
  {
    // calculate column bounds - The right cell x coordinate (cell.maxX) is part of the cell area
    cell.minX = static_cast<int>(x);
    cell.maxX = static_cast<int>(x + (cell_width-1)); // cell_width-1 because n * cell_width = width

    // We only need to look for the field below the horizon -> calculate the first y value below the horizon
    const int horizon_height = static_cast<int>(std::max(
        getArtificialHorizon().point(cell.maxX).y,
        getArtificialHorizon().point(cell.minX).y));
    const int min_scan_y = Math::clamp(horizon_height, 0, height-1);

    int skipped = 0;
    int successive_green = 0;
    int row_number = 1;

    // iterate rows; scan from bottom to top until min_scan_y
    for(double y = height-1; y - (cell_height-1) >= min_scan_y; y -= cell_height)
    {
      // calculate row bounds
      cell.maxY = static_cast<int>(y);
      cell.minY = static_cast<int>(y - (cell_height-1));

      // check if the cell is mostly green
      const double cell_green_density = cell.calc_green_density(getBallDetectorIntegralImage());
      const bool cell_is_green = cell_green_density >= params.proportion_of_green;

      DEBUG_REQUEST("Vision:IntegralFieldDetector:draw_grid",
        IMAGE_DRAWING_CONTEXT;
        CANVAS((cameraID==CameraInfo::Top? "ImageTop": "ImageBottom"));
        std::string color = cell_is_green? "00FF00": "FF00FF";
        PEN(color, 1);
        BOX(cell.minX, cell.minY, cell.maxX, cell.maxY);
      );

      if(cell_is_green) {
        skipped = 0;
        ++successive_green;

        // We found a field cell if all previous cells or the last params.min_successive_green cells were green
        if (successive_green >= params.min_successive_green || successive_green == row_number) {
          last_green_cell = cell;
          prev_was_green = true;
          green_found = true;
        }
      } else {
        // Cell may contain the field boundary -> check with a smaller threshold
        if(prev_was_green && cell_green_density >= params.end_proportion_of_green) {
          last_green_cell = cell;
        }
        ++skipped;
        // We may allow a number of cells to be skipped (e.g. cells lying on white field lines)
        if (skipped > params.max_skip_cells) {
          successive_green = 0;
        }
        prev_was_green = false;
      }
      row_number++;
    }

    // determing the endpoint
    if(green_found){
      // Additionally: Left most cell -> Determine the left most field endpoint
      if(is_first_column) {
        Vector2i left_endpoint;
        find_endpoint(last_green_cell.minX, last_green_cell, left_endpoint);
        endpoints.push_back(left_endpoint);
        is_first_column = false;
      }
      Vector2i endpoint;
      find_endpoint((last_green_cell.minX + last_green_cell.maxX) / 2,
                    last_green_cell, endpoint);
      endpoints.push_back(endpoint);
    }
  } // end scan

  // Right most cell -> Determine the right most endpoint so that there is no gap
  if(green_found) {
    Vector2i right_endpoint;
    find_endpoint(last_green_cell.maxX, last_green_cell, right_endpoint);
    endpoints.push_back(right_endpoint);
  }

  if(endpoints.size() > 1) {
    create_field();
  }
}

/**
 * Calculate the y value with the best green score inside the given cell and assign it to endpoint
 */
void IntegralFieldDetector::find_endpoint(int x, const Cell& cell, Vector2i& endpoint) {
  double score = 0.;
  double best_score = 0.;
  endpoint.x = x;
  endpoint.y = cell.maxY;
  // scan up and increase score if pixel is green, otherwise decrease score
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
