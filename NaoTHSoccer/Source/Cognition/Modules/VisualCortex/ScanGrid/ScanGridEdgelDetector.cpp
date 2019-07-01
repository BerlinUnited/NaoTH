#include "ScanGridEdgelDetector.h"

// tools
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/CameraGeometry.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

ScanGridEdgelDetector::ScanGridEdgelDetector()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_edgels",
                         "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_double_edgels",
                         "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_jump_vertical",
                         "mark brightness jumps on image", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
                         "mark brightness jumps on image", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_field_intersections",
                         "mark field poly intersections of the scanlines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:scanlines",
                         "draw activated scanlines", false);

  getDebugParameterList().add(&parameters);
}

ScanGridEdgelDetector::~ScanGridEdgelDetector()
{
  getDebugParameterList().remove(&parameters);
}

void ScanGridEdgelDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getScanLineEdgelPercept().reset();

  if(!getFieldPercept().valid) {
    return;
  }
  // TODO: implement a general validation for timestamps
  if(getBodyContour().timestamp != getFrameInfo().getTime()) {
    return;
  }

  // threshold
  int t_edge = cameraID == CameraInfo::Top? parameters.brightness_threshold_top:
                                            parameters.brightness_threshold_bottom;

  // initialize the scanner
  MaxPeakScan maximumPeak(t_edge);
  MinPeakScan minimumPeak(t_edge);

  scan_id = 0;

  if(parameters.scan_vertical) {
    scan_vertical(maximumPeak, minimumPeak);
    scan_id++;
  }

  if(parameters.scan_horizontal) {
    maximumPeak.reset();
    minimumPeak.reset();

    scan_horizontal(maximumPeak, minimumPeak);
  }

  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];
      LINE_PX(ColorClasses::black, edgel.point.x, edgel.point.y,
              edgel.point.x + (int)(edgel.direction.x*10),
              edgel.point.y + (int)(edgel.direction.y*10));
    }
  );
  // mark finished valid edgels
  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_double_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPercept().pairs[i];
      CIRCLE_PX(ColorClasses::black, (int)pair.point.x, (int)pair.point.y, 3);
      LINE_PX(ColorClasses::red, (int)pair.point.x, (int)pair.point.y,
              (int)(pair.point.x + pair.direction.x*10),
              (int)(pair.point.y + pair.direction.y*10));
    }
  );
}//end execute

void ScanGridEdgelDetector::scan_vertical(MaxPeakScan& maximumPeak,
                                          MinPeakScan& minimumPeak)
{
  // construct field poly line
  const std::vector<Vector2i>& poly_points = getFieldPercept().getField().getPoints();
  size_t poly_idx = find_min_point_x(poly_points);

  Math::LineSegment polyLine;
  poly_idx = next_poly_line(poly_idx, polyLine, poly_points);

  int x, y, luma, prevLuma, gradient, prevPoint;

  // vertical scanlines
  for(const ScanGrid::VScanLine scanline: getScanGrid().vertical)
  {
    bool begin_found = false;
    x = scanline.x;
    y = getScanGrid().vScanPattern[scanline.bottom];

    if(x < polyLine.begin().x) {
      // scanline is left of field poly
      continue;
    }

    // determine current field poly line
    bool right_of_field = false;
    while(x > polyLine.end().x)
    {
      if(polyLine.end().x < polyLine.begin().x) {
        // scanline is right of field poly
        right_of_field = true;
        break;
      }
      // construct next field poly line
      poly_idx = next_poly_line(poly_idx, polyLine, poly_points);
    }
    if(right_of_field) {
      continue;
    }

    // determine scanline intersection with field poly
    // if end_of_field is 0 there is no intersection
    int end_of_field = 0;

    double min_poly_y = polyLine.begin().y;
    double max_poly_y = polyLine.end().y;
    if(min_poly_y > max_poly_y) {
      std::swap(min_poly_y, max_poly_y);
    }

    if (getScanGrid().vScanPattern[scanline.bottom] < min_poly_y) {
      // scanline is certainly outside of field poly
      continue;
    }

    if (getScanGrid().vScanPattern[scanline.top] <= max_poly_y)
    {
      // scanline might intersect with field poly
      Vector2d begin(x, y);
      Vector2d end(x, getScanGrid().vScanPattern[scanline.top]);
      Math::LineSegment line(begin, end);

      if(line.intersect(polyLine)) {
        double t = line.intersection(polyLine);

        // set end of field to intersection
        Vector2d intersection = line.point(t);
        end_of_field = (int) intersection.y;

        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_field_intersections",
          CIRCLE_PX(ColorClasses::orange, x, end_of_field, 2);
        );
      } else if(getScanGrid().vScanPattern[scanline.bottom] < max_poly_y) {
        // scanline is outside of field poly
        continue;
      } // else scanline is inside of field poly
    }

    DEBUG_REQUEST("Vision:ScanGridEdgelDetector:scanlines",
      int top = std::max(getScanGrid().vScanPattern.at(scanline.top), end_of_field);
      LINE_PX(ColorClasses::white,
              scanline.x, getScanGrid().vScanPattern.at(scanline.bottom),
              scanline.x, top);
      for(size_t i=scanline.bottom; i<=scanline.top; ++i)
      {
        if (getScanGrid().vScanPattern.at(i) < top) {
          continue;
        }
        POINT_PX(ColorClasses::red, scanline.x, getScanGrid().vScanPattern.at(i));
      }
    );

    prevLuma = getImage().getY(scanline.x, y);

    int end_of_body =
        getBodyContour().getFirstFreeCell(Vector2i(scanline.x, y)).y;

    prevPoint = end_of_body;

    for(size_t i = scanline.bottom; i <= scanline.top; ++i) {
      y = getScanGrid().vScanPattern[i];
      if(y < end_of_field) {
        // don't scan above field area
        break;
      }

      luma = getImage().getY(x, y);
      if(y >= end_of_body) {
        prevLuma = luma;
        continue;
      }

      gradient = luma - prevLuma;

      // begin
      if(maximumPeak.add(y, prevPoint, gradient)) {
        // found greatest peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_vertical",
          LINE_PX(ColorClasses::red, x, maximumPeak.prev_point, x, maximumPeak.point);
        );

        if(std::abs(prevPoint - y) <= 2)
        {
          refine_vertical(maximumPeak, x);
          add_edgel(x, maximumPeak.point);
          begin_found = true;

        } else if(refine_range_vertical(maximumPeak, x))
        {
          add_edgel(x, maximumPeak.point);
          begin_found = true;
        }
        maximumPeak.reset();
      }

      // end
      if(minimumPeak.add(y, prevPoint, gradient)) {
        // found greatest peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_vertical",
          LINE_PX(ColorClasses::yellow, x, minimumPeak.prev_point, x, minimumPeak.point);
        );
        if(std::abs(prevPoint - y) <= 2)
        {
          refine_vertical(maximumPeak, x);
          add_edgel(x, minimumPeak.point);
          // found a new double edgel
          if(begin_found) {
            add_double_edgel(scan_id);
          }
          begin_found = false;
        } else if(refine_range_vertical(maximumPeak, x))
        {
          add_edgel(x, minimumPeak.point);
          // found a new double edgel
          if(begin_found) {
            add_double_edgel(scan_id);
          }
          begin_found = false;
        }
        minimumPeak.reset();
      }

      prevLuma = luma;
      prevPoint = y;
    }
    ++scan_id;
  }
}// end scan_vertical

inline void ScanGridEdgelDetector::refine_vertical(MaxPeakScan& maximumPeak, int x) {
  // refine the position of the peak
  int f0 = getImage().getY(x, maximumPeak.point);
  if (maximumPeak.point+2 < getImage().height()) {
    int f2 = getImage().getY(x, maximumPeak.point+2);
    maximumPeak.check(maximumPeak.point+1, f0-f2);
  }
  if(maximumPeak.point-2 >= 0) {
    int f_2 = getImage().getY(x, maximumPeak.point-2);
    maximumPeak.check(maximumPeak.point-1, f_2-f0);
  }
}

inline bool ScanGridEdgelDetector::refine_range_vertical(MaxPeakScan& maximumPeak, int x) {
  int height = getImage().height();
  int start = std::min(maximumPeak.prev_point, height-2);
  int end = std::max(maximumPeak.point-1, 0);
  maximumPeak.reset();

  int prev = std::min(start+2, height-1);
  int luma, gradient;
  int prevLuma = getImage().getY(x, prev);

  for(int y=start; y>=end; y-=2) {
    luma = getImage().getY(x, y);
    gradient = luma - prevLuma;
    if(maximumPeak.add(y+1, prev, gradient)) {
      break;
    }
    prev = y;
    prevLuma = luma;
  }
  if(maximumPeak.found) {
    refine_vertical(maximumPeak, x);
    return true;
  }
  return false;
} // end refine_range_vertical

void ScanGridEdgelDetector::scan_horizontal(MaxPeakScan& maximumPeak,
                                            MinPeakScan& minimumPeak)
{
  // construct field poly lines
  const std::vector<Vector2i>& poly_points = getFieldPercept().getField().getPoints();

  size_t poly_idx_left = find_min_point_y(poly_points);
  size_t poly_idx_right = poly_idx_left;

  Math::LineSegment polyLineLeft;
  poly_idx_left = prev_poly_line(poly_idx_left, polyLineLeft, poly_points);

  Math::LineSegment polyLineRight;
  poly_idx_right = next_poly_line(poly_idx_right, polyLineRight, poly_points);

  /*DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_field_intersections",
    LINE_PX(ColorClasses::yellow, (int) polyLineLeft.begin().x, (int) polyLineLeft.begin().y,
                                  (int) polyLineLeft.end().x, (int) polyLineLeft.end().y);
    LINE_PX(ColorClasses::green, (int) polyLineRight.begin().x, (int) polyLineRight.begin().y,
                                 (int) polyLineRight.end().x, (int) polyLineRight.end().y);
  );*/

  int x, y, end_x, luma, prevLuma, gradient, prevPoint;

  // horizontal scanlines
  for(const ScanGrid::HScanLine scanline: getScanGrid().horizontal)
  {
    x = scanline.left_x;
    y = scanline.y;
    end_x = scanline.right_x;

    // determine scanline intersections with the field poly line
    if(y < polyLineLeft.begin().y) {
      // scanline is outside of field polygon
      continue;
    } else {
      // create scanline line segment
      Vector2d begin(scanline.left_x, scanline.y);
      Vector2d end(scanline.right_x, scanline.y);
      Math::LineSegment line(begin, end);

      // determine current left field poly line
      bool under_field = false;
      while(y > polyLineLeft.end().y)
      {
        if(polyLineLeft.end().x > polyLineLeft.begin().x) {
          // scanline is under field poly (shouldn't happen)
          under_field = true;
          break;
        }
        // construct next field poly line
        poly_idx_left = prev_poly_line(poly_idx_left, polyLineLeft, poly_points);
      }
      if(under_field) {
        // shouldn't happen
        break;
      }
      // determine start of scanline (intersection with left polyline)
      if(line.intersect(polyLineLeft))
      {
        double t = line.intersection(polyLineLeft);

        // set start of scanline to intersection
        Vector2d intersection = line.point(t);
        x = (int) intersection.x;

        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_field_intersections",
          CIRCLE_PX(ColorClasses::red, x, scanline.y, 3);
        );
      }

      // determine current right field poly line
      under_field = false;
      while(y > polyLineRight.end().y)
      {
        if(polyLineRight.end().x < polyLineRight.begin().x) {
          // scanline is under field poly (shouldn't happen)
          under_field = true;
          break;
        }
        // construct next field poly line
        poly_idx_right = next_poly_line(poly_idx_right, polyLineRight, poly_points);
      }
      if(under_field) {
        // shouldn't happen
        break;
      }
      // determine end of scanline (intersection with right polyline)
      if(line.intersect(polyLineRight))
      {
        double t = line.intersection(polyLineRight);

        // set end of scanline to intersection
        Vector2d intersection = line.point(t);
        end_x = (int) intersection.x;

        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_field_intersections",
          CIRCLE_PX(ColorClasses::pink, end_x, scanline.y, 2);
        );
      }
    }

    bool begin_found = false;

    prevLuma = getImage().getY(x, y);
    prevPoint = x;
    for(;x <= end_x; x += scanline.skip) {

      luma = getImage().getY(x, y);
      gradient = luma - prevLuma;

      // begin
      if(maximumPeak.add(x, prevPoint, gradient)) {
        // found greatest peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
          LINE_PX(ColorClasses::red, maximumPeak.prev_point, y, maximumPeak.point, y);
        );
        if(std::abs(prevPoint - x) <= 2)
        {
          refine_horizontal(maximumPeak, y);
          if(!getBodyContour().isOccupied(maximumPeak.point,y)) {
            add_edgel(maximumPeak.point, y);
            begin_found = true;
          }
        } else if(refine_range_horizontal(maximumPeak, y) && !getBodyContour().isOccupied(maximumPeak.point,y)) {
          add_edgel(maximumPeak.point, y);
          begin_found = true;
        }
        maximumPeak.reset();
      }

      // end
      if(minimumPeak.add(x, prevPoint, gradient)) {
        // found greatest peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
          LINE_PX(ColorClasses::yellow, minimumPeak.prev_point, y, minimumPeak.point, y);
        );
        if(std::abs(prevPoint - x) <= 2)
        {
          refine_horizontal(maximumPeak, y);
          if(!getBodyContour().isOccupied(minimumPeak.point,y)) {
            add_edgel(minimumPeak.point, y);
            // found a new double edgel
            if(begin_found) {
              add_double_edgel(scan_id);
            }
            begin_found = false;
          }
        } else if(refine_range_horizontal(minimumPeak, y) && !getBodyContour().isOccupied(minimumPeak.point,y)) {
          add_edgel(minimumPeak.point, y);
          // found a new double edgel
          if(begin_found) {
            add_double_edgel(scan_id);
          }
          begin_found = false;
        }
        minimumPeak.reset();
      }

      prevLuma = luma;
      prevPoint = x;
    }
    ++scan_id;
  }
} // end scan_horizontal

inline void ScanGridEdgelDetector::refine_horizontal(MaxPeakScan& maximumPeak, int y) {
  // refine the position of the peak
  int f0 = getImage().getY(maximumPeak.point, y);
  if(maximumPeak.point-2 >= 0) {
    int f_2 = getImage().getY(maximumPeak.point-2, y);
    maximumPeak.check(maximumPeak.point-1, f_2-f0);
  }
  if(maximumPeak.point+2 < getImage().width()) {
    int f2 = getImage().getY(maximumPeak.point+2, y);
    maximumPeak.check(maximumPeak.point+1, f0-f2);
  }
}

inline bool ScanGridEdgelDetector::refine_range_horizontal(MaxPeakScan& maximumPeak, int y) {
  int width = getImage().width();
  int start = std::max(maximumPeak.prev_point, 1);
  int end = std::min(maximumPeak.point+1, width-1);
  maximumPeak.reset();

  int prev = std::max(start-2, 0);
  int luma, gradient;
  int prevLuma = getImage().getY(prev, y);

  for(int x=start; x<=end; x+=2) {
    luma = getImage().getY(x, y);
    gradient = luma - prevLuma;
    if(maximumPeak.add(x-1, prev, gradient)) {
      break;
    }
    prev = x;
    prevLuma = luma;
  }
  if(maximumPeak.found) {
    refine_horizontal(maximumPeak, y);
    return true;
  }
  return false;
} // end refine_range_horizontal

Vector2d ScanGridEdgelDetector::calculateGradient(const Vector2i& point) const
{
  Vector2d gradient;
  static const int offset = 1;

  // no angle at the border (shouldn't happen)
  if( point.x < offset || point.x + offset + 1 > (int)getImage().width() ||
      point.y < offset || point.y + offset + 1 > (int)getImage().height() ) {
    return gradient;
  }

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means

  const int x0 = point.x-offset;
  const int y0 = point.y-offset;
  const int x1 = point.x+offset;
  const int y1 = point.y+offset;

  // NOTE: char type is converted to int before doing the actual calculations
  //       so we don't need cast here
  gradient.x =
       getImage().getY_direct(x0, y1)
    +2*getImage().getY_direct(point.x, y1)
    +  getImage().getY_direct(x1, y1)
    -  getImage().getY_direct(x0, y0)
    -2*getImage().getY_direct(point.x, y0)
    -  getImage().getY_direct(x1, y0);

  gradient.y =
       getImage().getY_direct(x0, y0)
    +2*getImage().getY_direct(x0, point.y)
    +  getImage().getY_direct(x0, y1)
    -  getImage().getY_direct(x1, y0)
    -2*getImage().getY_direct(x1, point.y)
    -  getImage().getY_direct(x1, y1);


  //calculate the angle of the gradient
  return gradient.normalize();
} // end calculateGradient
