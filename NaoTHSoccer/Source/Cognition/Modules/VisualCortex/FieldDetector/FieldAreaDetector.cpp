#include "FieldAreaDetector.h"
#include <cmath>

#include <Tools/Math/ConvexHull.h>

FieldAreaDetector::FieldAreaDetector()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:FieldAreaDetector:draw_grid", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldAreaDetector:draw_best_squares", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldAreaDetector:draw_split", "", false);
  DEBUG_REQUEST_REGISTER("Vision:FieldAreaDetector:mark_field_polygon", "", false);
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

  endpoints.clear();

  horizon_height = (int) std::max(getArtificialHorizon().begin().y+1.5,
                                  getArtificialHorizon().end().y+1.5);
  horizon_height = std::max(0, horizon_height/factor);

  grid_size = params.grid_size/factor;
  int n_cells_horizontal = getBallDetectorIntegralImage().getWidth()/grid_size;
  int offset = (getBallDetectorIntegralImage().getWidth() - (n_cells_horizontal*grid_size)) / n_cells_horizontal;
  pixels_per_cell = ((grid_size+1)*(grid_size+1));
  min_green = (int)(pixels_per_cell*params.proportion_of_green);

  Cell cell;
  Cell last_green_cell;

  double x_offset = 0;

  for (cell.minX=offset; cell.minX < (int)getBallDetectorIntegralImage().getWidth(); cell.minX += grid_size+offset) {
    //cell.maxX = cell.minX + grid_size;
    //if (cell.maxX >= (int)getBallDetectorIntegralImage().getWidth()) break;
    cell.maxX = std::min(cell.minX + grid_size, (int)getBallDetectorIntegralImage().getWidth()-1);
 
    // get first pos not occupied by body. HACK: This does not take the difference in refinement into consideration
    Vector2i start_left(cell.minX*factor, getImage().height()-1);
    Vector2i start_right(cell.maxX*factor, getImage().height()-1);
    int start_y = std::min(getBodyContour().getFirstFreeCell(start_left).y/factor,
                           getBodyContour().getFirstFreeCell(start_right).y/factor);

    bool green_found = false;
    bool isLastCell = false;
    for(cell.minY = getBallDetectorIntegralImage().getHeight()-1-grid_size; !isLastCell; cell.minY -= grid_size) {
      if(cell.minY < horizon_height) {
        cell.minY = horizon_height;
        isLastCell = true;
      }
      cell.maxY = cell.minY + grid_size;
      if(cell.maxY > start_y) {
        continue;
      }
      cell.sum_of_green = getBallDetectorIntegralImage().getSumForRect(cell.minX, cell.minY, cell.maxX, cell.maxY, 1);
      if(cell.sum_of_green >= min_green) {
        last_green_cell = cell;
        green_found = true;
      } else {
        break;
      }
      DEBUG_REQUEST("Vision:FieldAreaDetector:draw_grid",
        if(cell.sum_of_green >= min_green) {
          RECT_PX(ColorClasses::green, cell.minX*factor+1, cell.minY*factor+1, cell.maxX*factor-1, cell.maxY*factor-1);
        } else {
          RECT_PX(ColorClasses::red, cell.minX*factor+1, cell.minY*factor+1, cell.maxX*factor-1, cell.maxY*factor-1);
        }
      );
    }

    if(green_found) {
      int ix_offset = (int) std::round(x_offset);

      if (params.refine_cell) {
        refine_cell(last_green_cell);
      }      
      Cell upper, lower;
      int half_grid_size = split_cell(last_green_cell, upper, lower, ix_offset);
      int min_green_half = (int)(params.proportion_of_green * (half_grid_size+1) * (half_grid_size+1));
      Cell endpoint_cell;
      if (upper.sum_of_green >= min_green_half && lower.sum_of_green >= min_green_half) {
        endpoint_cell = upper;
      } else {
        endpoint_cell = lower;
      }

      Endpoint endpoint;
      endpoint.cameraID = cameraID;
      find_endpoint(endpoint_cell, endpoint, ix_offset);
      if (params.refine_point) {
        refine_point(endpoint, endpoint_cell.minY);
      }
      endpoints.push_back(endpoint);
    }
    x_offset += ((double)(grid_size) / 2 / n_cells_horizontal);
  }
  create_field();
}

void FieldAreaDetector::find_endpoint(const Cell& cell, Endpoint& endpoint, int point_x_offset) {
  int grid_size = cell.maxY - cell.minY;
  int pixels_per_cell = (grid_size+1) * (grid_size+1);

  double cut = cell.sum_of_green / (double)pixels_per_cell;
  endpoint.pos.x = (cell.minX + point_x_offset) * factor; //(cell.minX+cell.maxX)/2 * factor;
  endpoint.pos.y = (cell.maxY-(int)(cut*grid_size)) * factor;

  DEBUG_REQUEST("Vision:FieldAreaDetector:draw_best_squares",
    RECT_PX(ColorClasses::blue, cell.minX*factor+2, cell.minY*factor+2,
      cell.maxX*factor-2, cell.maxY*factor-2);
    RECT_PX(ColorClasses::orange, endpoint.pos.x-1, endpoint.pos.y-1, endpoint.pos.x+1, endpoint.pos.y+1);
  );
}

int32_t FieldAreaDetector::split_cell(const Cell& cell, Cell& upper, Cell& lower, int split_x_offset) {
  int32_t half_grid_size = grid_size/2;
  upper.minX = lower.minX = cell.minX + split_x_offset;// half_grid_size/2;
  upper.maxX = lower.maxX = lower.minX + half_grid_size;

  lower.minY = cell.maxY - half_grid_size;
  lower.maxY = cell.maxY;
  lower.sum_of_green = getBallDetectorIntegralImage().getSumForRect(lower.minX, lower.minY, lower.maxX, lower.maxY, 1);

  upper.maxY = lower.minY;
  upper.minY = lower.maxY - grid_size;
  upper.sum_of_green = getBallDetectorIntegralImage().getSumForRect(upper.minX, upper.minY, upper.maxX, upper.maxY, 1);

  DEBUG_REQUEST("Vision:FieldAreaDetector:draw_split",
    ColorClasses::Color color;
    if(upper.sum_of_green >= ((half_grid_size+1)*(half_grid_size+1)*params.proportion_of_green)) {
      color = ColorClasses::green;
    } else {
      color = ColorClasses::red;
    }
    RECT_PX(color, upper.minX*factor+1, upper.minY*factor+1, upper.maxX*factor-1, upper.maxY*factor-1);
    if(lower.sum_of_green >= ((half_grid_size+1)*(half_grid_size+1)*params.proportion_of_green)) {
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
  for(int offset = 0; offset < grid_size && cell.minY-offset >= horizon_height; ++offset) {
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

void FieldAreaDetector::refine_point(Endpoint& endpoint, int32_t minY) {
  Pixel pixel;
  for(int y=endpoint.pos.y; y>=minY && y>=horizon_height; --y) {
    pixel = getImage().get(endpoint.pos.x, y);
    if (getFieldColorPercept().greenHSISeparator.isColor(pixel)) {
      endpoint.pos.y = y;
    } else {
      break;
    }
  }
}

void FieldAreaDetector::create_field() {
  std::vector<Vector2i> points;

  for(size_t i = 0; i < endpoints.size(); i++)
  {
    if (endpoints[i].pos.y != -1) {
      points.push_back(endpoints[i].pos);
    }
  }

  if(points.size() > 1)
  {
    Vector2i p1 = points.front();
    p1.y = getImage().height() - 1;
    Vector2i p2 = points.back();
    p2.y = getImage().height() - 1;

    points.push_back(p1);
    points.push_back(p2);
  }

  std::vector<Vector2i> result = ConvexHull::convexHull(points);
  FieldPercept::FieldPoly fieldPoly;

  for(size_t i = 0; i < result.size(); i++)
  {
    fieldPoly.add(result[i]);
  }

  DEBUG_REQUEST("Vision:FieldAreaDetector:mark_field_polygon",
    int idx = 0;
    for(int i = 1; i < fieldPoly.length; i++)
    {
      LINE_PX(ColorClasses::red, fieldPoly[idx].x, fieldPoly[idx].y, fieldPoly[i].x, fieldPoly[i].y);
      idx = i;
    }
  );
}
