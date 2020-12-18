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
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:scanlines:vertical",
                         "draw activated scanlines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:scanlines:horizontal",
                         "draw activated scanlines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:draw_refinement", "", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:plot_gradient_vertical_scan", "", false);

  getDebugParameterList().add(&params);
}

ScanGridEdgelDetector::~ScanGridEdgelDetector()
{
  getDebugParameterList().remove(&params);
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
  int t_edge = cameraID == CameraInfo::Top? params.brightness_threshold_top:
      params.brightness_threshold_bottom;

  // initialize the scanner
  MaxPeakScan maximumPeak(t_edge);
  MinPeakScan minimumPeak(t_edge);

  scan_id = 0;

  if(params.scan_vertical) {
    scan_vertical(maximumPeak, minimumPeak);
    // HACK: increase scan_id here so vertical and horizontal scans are one id appart
    // so that LineGraphProvider doesn't match vertical scan edgels with horizontal scan edgels
    scan_id++;
  }

  if(params.scan_horizontal) {
    maximumPeak.reset();
    minimumPeak.reset();

    scan_horizontal(maximumPeak, minimumPeak);
  }

  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];
      ColorClasses::Color color = ColorClasses::black;
      if(edgel.type == Edgel::positive) {
        color = ColorClasses::blue;
      } else if(edgel.type == Edgel::negative) {
        color = ColorClasses::red;
      }
      LINE_PX(color, edgel.point.x, edgel.point.y,
              edgel.point.x + (int)(edgel.direction.x*5),
              edgel.point.y + (int)(edgel.direction.y*5));
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

  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_field_intersections",
    LINE_PX(ColorClasses::yellow, (int) polyLine.begin().x, (int) polyLine.begin().y,
                                  (int) polyLine.end().x, (int) polyLine.end().y);
  );

  // vertical scanlines
  for(const ScanGrid::VScanLine scanline: getScanGrid().vertical)
  {
    int x = scanline.x;
    int y = getScanGrid().vScanPattern[scanline.bottom];

    if(x - params.gradient_offset < 0 || x + params.gradient_offset >= static_cast<int>(getImage().width())) {
      // cannot extract edgel direction on image border
      continue;
    }

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
    int end_of_field = 0;

    Vector2d begin(x, y);
    //Vector2d end(x, getScanGrid().vScanPattern[scanline.top]);
    Vector2d end(x, 0);
    Math::LineSegment line(begin, end);

    if(line.intersect(polyLine)) {
      double t = line.intersection(polyLine);

      // set end of field to intersection
      Vector2d intersection = line.point(t);
      end_of_field = (int) intersection.y;

      DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_field_intersections",
        CIRCLE_PX(ColorClasses::orange, x, end_of_field, 2);
      );
    } else {
      // scanline is outside of field poly
      continue;
    }

    DEBUG_REQUEST("Vision:ScanGridEdgelDetector:scanlines:vertical",
      IMAGE_DRAWING_CONTEXT;
      CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
      int top = std::max(getScanGrid().vScanPattern.at(scanline.top), end_of_field);
      PEN("FFFFFF", 2);
      LINE(scanline.x, getScanGrid().vScanPattern.at(scanline.bottom),
           scanline.x, top);
      PEN("FF0000", 2);
      for(size_t i=scanline.bottom; i<=scanline.top; ++i)
      {
        if (getScanGrid().vScanPattern.at(i) < top) {
          continue;
        }
        FILLOVAL(scanline.x, getScanGrid().vScanPattern.at(i), 1, 1);
      }
    );

    int end_of_body = getBodyContour().getFirstFreeCell(Vector2i(scanline.x, y)).y;

    // TODO: cleanup
    if(getScanGrid().vScanPattern.size() < 2) {
      // need at least two scan pattern points to calculate last_scan_pattern_gap, TODO: use min gap (last_scan_pattern_gap = 2)
      return;
    }
    int last_scan_pattern_gap = getScanGrid().vScanPattern.at(getScanGrid().vScanPattern.size()-2)
                                - getScanGrid().vScanPattern.back();

    bool begin_found = false;
    minimumPeak.reset();
    maximumPeak.reset();

    // TODO: Is this correct or can we do better?
    int prev_y = getScanGrid().vScanPattern[scanline.bottom];
    int prevLuma = getImage().getY(scanline.x, prev_y);

    // TODO: cleanup
    size_t i = scanline.bottom;
    for(y = getScanGrid().vScanPattern[i]; y >= end_of_field; y = (++i < getScanGrid().vScanPattern.size())? getScanGrid().vScanPattern[i]: y - last_scan_pattern_gap) {
      int luma = getImage().getY(x, y);

      // TODO: "y >" should be enough
      if(y >= end_of_body) {
        // don't scan inside own body
        prev_y = y;
        prevLuma = luma;
        continue;
      }

      // calculate gradient
      int check_y = (prev_y + y)/2;
      int gradient = luma - prevLuma;

      DEBUG_REQUEST("Vision:ScanGridEdgelDetector:plot_gradient_vertical_scan",
        std::ostringstream os;
        os << ((cameraID==CameraInfo::Top)? "GradiantTop": "GradiantBottom") << " x=" << x;
        std::cout << os.str() << " " << i-scanline.bottom << " = " << check_y << " " << minimumPeak.found << std::endl;
        PLOT_GENERIC(os.str(), static_cast<int>(i-scanline.bottom), gradient);
      );

      // begin
      if(maximumPeak.add(check_y, prev_y, y, gradient)) {
        // found local maximum peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:plot_gradient_vertical_scan",
          std::cout << "x=" << x << " max " << check_y << " point=" << maximumPeak.point << std::endl;
        );
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_vertical",
          LINE_PX(ColorClasses::red, x, maximumPeak.prev_point, x, maximumPeak.next_point);
        );

        if(prev_y - y <= 2)
        {
          // gap is small, just make a simple refinement
          refine_vertical(maximumPeak, x);
          // add edgel to percept
          bool success = add_edgel(x, maximumPeak.point, Edgel::positive);
          if(success) {
            begin_found = true;
          }
        }
        else if(refine_range_vertical(maximumPeak, x))
        {
          // if peak found in range add edgel to percept
          bool success = add_edgel(x, maximumPeak.point, Edgel::positive);
          if(success) {
            begin_found = true;
          }
        }

        maximumPeak.reset();
      }

      // end
      if(minimumPeak.add(check_y, prev_y, y, gradient))
      {
        // found local minimum peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:plot_gradient_vertical_scan",
          std::cout <<  "x=" << x << " min " << check_y << " point=" << minimumPeak.point << std::endl;
        );
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_vertical",
          LINE_PX(ColorClasses::yellow, x, minimumPeak.prev_point, x, minimumPeak.next_point);
        );
        if(std::abs(prev_y - y) <= 2)
        {
          // gap is small, just make a simple refinement
          refine_vertical(minimumPeak, x);

          // add edgel to percept
          bool success = add_edgel(x, minimumPeak.point, Edgel::negative);
          if(success) {
            // found a new double edgel
            if(begin_found) {
              add_double_edgel(scan_id);
              begin_found = false;
            }
          }
        }
        else if(refine_range_vertical(minimumPeak, x))
        {
          // if peak found in range add edgel to percept
          bool success = add_edgel(x, minimumPeak.point, Edgel::negative);
          if(success) {
            // found a new double edgel
            if(begin_found) {
              add_double_edgel(scan_id);
              begin_found = false;
            }
          }

        }

        minimumPeak.reset();
      }

      prevLuma = luma;
      prev_y = y;
    }
    ++scan_id;
  }
}// end scan_vertical

inline void ScanGridEdgelDetector::refine_vertical(MaxPeakScan& maximumPeak, int x) {
  int height = getImage().height();

  // refine the position of the peak
  int f0 = getImage().getY(x, maximumPeak.point);
  if (maximumPeak.point+2 < height) {
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

  // improve the range so we can calculate a gradient on the first and last scan point
  int start = std::min(maximumPeak.prev_point, height-2);
  int end = std::max(maximumPeak.next_point-1, 0);

  maximumPeak.reset();

  int prev = std::min(start+2, height-1);
  int prevLuma = getImage().getY(x, prev);

  for(int y=start; y>=end; y-=2) {
    DEBUG_REQUEST("Vision:ScanGridEdgelDetector:draw_refinement",
      POINT_PX(ColorClasses::pink, x, y);
    );

    int luma = getImage().getY(x, y);
    int gradient = luma - prevLuma;
    // HACK: -1, -1 cause interval isn't of interest here
    if(maximumPeak.add(y+1, -1, -1, gradient)) {
      // we are just looking for one peak here
      break;
    }
    prevLuma = luma;
  }
  if(maximumPeak.found) {
    // we scanned the interval for every second pixel, so make another simple refinement
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

  // horizontal scanlines
  for(const ScanGrid::HScanLine scanline: getScanGrid().horizontal)
  {
    int x = scanline.left_x;
    int y = scanline.y;
    int end_x = scanline.right_x;

    if(y - params.gradient_offset < 0 || y + params.gradient_offset >= static_cast<int>(getImage().height())) {
      // cannot extract edgel direction on image border
      continue;
    }

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

    DEBUG_REQUEST("Vision:ScanGridEdgelDetector:scanlines:horizontal",
      IMAGE_DRAWING_CONTEXT;
      CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
      PEN("FFFFFF", 2);

      LINE(x, scanline.y, end_x, scanline.y);

      PEN("FF0000", 2);
      for(int xx = x; xx <= end_x; xx += scanline.skip) {
        if(!getBodyContour().isOccupied(xx, scanline.y)) {
          FILLOVAL(xx, scanline.y, 1, 1);
        }
      }
    );

    bool begin_found = false;
    minimumPeak.reset();
    maximumPeak.reset();

    int prev_x = x;
    int prevLuma = getImage().getY(prev_x, y);

    for(;x <= end_x; x += scanline.skip) {

      int luma = getImage().getY(x, y);
      int gradient = luma - prevLuma;

      int check_x = (prev_x + x)/2;

      // begin
      if(maximumPeak.add(check_x, prev_x, x, gradient)) {
        // found local maximum peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
          LINE_PX(ColorClasses::red, maximumPeak.prev_point, y, maximumPeak.next_point, y);
        );
        if(std::abs(prev_x - x) <= 2)
        {
          refine_horizontal(maximumPeak, y);
          if(!getBodyContour().isOccupied(maximumPeak.point, y)) {

            bool success = add_edgel(maximumPeak.point, y, Edgel::positive);
            if(success) {
              begin_found = true;
            }
          }
        } else if(refine_range_horizontal(maximumPeak, y) && !getBodyContour().isOccupied(maximumPeak.point,y)) {

          bool success = add_edgel(maximumPeak.point, y, Edgel::positive);
          if(success) {
            begin_found = true;
          }
        }
        maximumPeak.reset();
      }

      // end
      if(minimumPeak.add(check_x, prev_x, x, gradient)) {
        // found local minimum peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
          LINE_PX(ColorClasses::yellow, minimumPeak.prev_point, y, minimumPeak.next_point, y);
        );
        if(std::abs(prev_x - x) <= 2)
        {
          refine_horizontal(minimumPeak, y);
          if(!getBodyContour().isOccupied(minimumPeak.point,y)) {

            bool success = add_edgel(minimumPeak.point, y, Edgel::negative);

            if(success) {
              // found a new double edgel
              if(begin_found) {
                add_double_edgel(scan_id);
              }
              begin_found = false;
            }
          }

        } else if(refine_range_horizontal(minimumPeak, y) && !getBodyContour().isOccupied(minimumPeak.point,y)) {

          bool success = add_edgel(minimumPeak.point, y, Edgel::negative);

          if(success) {
            // found a new double edgel
            if(begin_found) {
              add_double_edgel(scan_id);
            }
            begin_found = false;
          }

        }
        minimumPeak.reset();
      }

      prevLuma = luma;
      prev_x = x;
    }
    ++scan_id;
  }
} // end scan_horizontal

inline void ScanGridEdgelDetector::refine_horizontal(MaxPeakScan& maximumPeak, int y) {
  int width = getImage().width();

  // refine the position of the peak
  int f0 = getImage().getY(maximumPeak.point, y);
  if(maximumPeak.point-2 >= 0) {
    int f_2 = getImage().getY(maximumPeak.point-2, y);
    maximumPeak.check(maximumPeak.point-1, f_2-f0);
  }
  if(maximumPeak.point+2 < width) {
    int f2 = getImage().getY(maximumPeak.point+2, y);
    maximumPeak.check(maximumPeak.point+1, f0-f2);
  }
}

inline bool ScanGridEdgelDetector::refine_range_horizontal(MaxPeakScan& maximumPeak, int y) {
  int width = getImage().width();

  // improve the range so we can calculate a gradient on the first and last scan point
  int start = std::max(maximumPeak.prev_point, 1);
  int end = std::min(maximumPeak.next_point+1, width-1);
  maximumPeak.reset();

  int prev = std::max(start-2, 0);
  int prevLuma = getImage().getY(prev, y);

  for(int x=start; x<=end; x+=2) {
    int luma = getImage().getY(x, y);
    int gradient = luma - prevLuma;
    // HACK: -1, -1 cause interval isn't of interest here
    if(maximumPeak.add(x-1, -1, -1, gradient)) {
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
  static const int offset = params.gradient_offset;

  // no angle at the border (shouldn't happen)
  if( point.x < offset || point.x + offset + 1 > (int)getImage().width() ||
      point.y < offset || point.y + offset + 1 > (int)getImage().height() ) {
    ASSERT(false);
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
