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
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_double_edgels_field",
                         "Project edgel positions onto the field", false);
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
    IMAGE_DRAWING_CONTEXT;
    CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));

    for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];

      PEN("000000", 1);
      if(edgel.type == Edgel::positive) {
        PEN("FF0000", 1);
      } else if(edgel.type == Edgel::negative) {
        PEN("00FFFF", 1);
      }
      LINE(edgel.point.x - (int)(edgel.direction.x*5),
           edgel.point.y - (int)(edgel.direction.y*5),
           edgel.point.x + (int)(edgel.direction.x*5),
           edgel.point.y + (int)(edgel.direction.y*5));
    }
  );

  // mark finished valid edgels
  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_double_edgels",
    IMAGE_DRAWING_CONTEXT;
    CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPercept().pairs[i];
      PEN("000000", 1);
      CIRCLE((int)pair.point.x, (int)pair.point.y, 3);
      PEN("FF66DD", 1);
      LINE((int)pair.point.x, (int)pair.point.y,
           (int)(pair.point.x + pair.direction.x*10),
           (int)(pair.point.y + pair.direction.y*10));
    }
  );

  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_double_edgels_field",
    FIELD_DRAWING_CONTEXT;
    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPercept().pairs[i];
      Vector2d field_point;
      CameraGeometry::imagePixelToFieldCoord(getCameraMatrix(),
                                             getCameraInfo(),
                                             pair.point.x,
                                             pair.point.y,
                                             0.0,
                                             field_point);
      PEN("000000", 1);
      TEXT_DRAWING2(field_point.x+25, field_point.y+25, 0.1, pair.id);
      PEN("000000", 2);
      CIRCLE(field_point.x, field_point.y, 10);
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
  for(scan_id = 0; scan_id < static_cast<int>(getScanGrid().vertical.size()); ++scan_id)
  {
    const ScanGrid::VScanLine& scanline = getScanGrid().vertical[scan_id];

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

    int end_of_body = getBodyContour().getFirstFreeCell(Vector2i(scanline.x, y)).y;

    DEBUG_REQUEST("Vision:ScanGridEdgelDetector:scanlines:vertical",
      IMAGE_DRAWING_CONTEXT;
      CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
      PEN("FFFFFF", 2);
      LINE(scanline.x, end_of_body, scanline.x, end_of_field);
    );

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

    y = end_of_body;
    for(size_t i = scanline.bottom + 1; true; ++i) {
      int prev_y;
      // get y values for gradient calculations
      if(i < getScanGrid().vScanPattern.size()) {
        prev_y = getScanGrid().vScanPattern[i-1];
        y = getScanGrid().vScanPattern[i];
      } else {
        prev_y = y;
        y -= last_scan_pattern_gap;
      }

      if(prev_y > end_of_body) {
        // don't scan own body
        continue;
      }

      DEBUG_REQUEST("Vision:ScanGridEdgelDetector:scanlines:vertical",
        PEN("FF0000", 2);
        FILLOVAL(x, y, 1, 1);
      );

      if(prev_y - y > 1)
      {
        // we are skipping pixels, do an interval scan
        if(y < end_of_field) {
          // reached the end
          break;
        }

        int check_y = (prev_y + y)/2;
        // gradient = next - previous
        int gradient = getImage().getY(x, y) - getImage().getY(x, prev_y);

        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:plot_gradient_vertical_scan",
          // HACK: Pad x values with 0 so entries are sorted correctly in robot control
          std::string padding = "";
          if (x < 10) {
            padding = "00";
          } else if (x < 100) {
            padding = "0";
          }
          std::string camera = (cameraID==CameraInfo::Top)? "GradiantTop": "GradiantBottom";
          std::ostringstream os;
          os << camera << " x=" << padding << x;
          // HACK: -check_y because robot control seems to require accending order
          PLOT_GENERIC(os.str(), -check_y, gradient);
        );

        if(maximumPeak.add(check_y, prev_y, y, gradient))
        {
          DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_vertical",
            IMAGE_DRAWING_CONTEXT;
            CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
            PEN("FF0000", 2);
            LINE(x, maximumPeak.prev_point, x, maximumPeak.next_point);
          );
          // found a peak, do a fine scan
          if(refine_vertical(maximumPeak, x))
          {
            // found an edgel
            add_edgel(x, maximumPeak.point, Edgel::positive);
            begin_found = true;
          }
          maximumPeak.reset();
        }

        if(minimumPeak.add(check_y, prev_y, y, gradient))
        {
          DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_vertical",
            IMAGE_DRAWING_CONTEXT;
            CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
            PEN("00FFFF", 2);
            LINE(x, minimumPeak.prev_point, x, minimumPeak.next_point);
          );
          // found a peak, do a fine scan
          if(refine_vertical(minimumPeak, x))
          {
            // found an edgel
            add_edgel(x, minimumPeak.point, Edgel::negative);
            if(begin_found) {
              // found a new double edgel
              add_double_edgel(scan_id, true);
              begin_found = false;
            }
          }
          minimumPeak.reset();
        }
      } else {
        // scan every pixel
        int next_y = y-1;
        if(next_y < end_of_field) {
          // reached the end
          break;
        }
        int gradient = getImage().getY(x, next_y) - getImage().getY(x, prev_y);

        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:plot_gradient_vertical_scan",
          // HACK: Pad x values with 0 so entries are sorted correctly in robot control
          std::string padding = "";
          if (x < 10) {
            padding = "00";
          } else if (x < 100) {
            padding = "0";
          }
          std::string camera = (cameraID==CameraInfo::Top)? "GradiantTop": "GradiantBottom";
          std::ostringstream os;
          os << camera << " x=" << padding << x;
          // HACK: -y because robot control seems to require accending order
          PLOT_GENERIC(os.str(), -y, gradient);
        );

        if(maximumPeak.add(y, prev_y, next_y, gradient))
        {
          DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_vertical",
            IMAGE_DRAWING_CONTEXT;
            CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
            PEN("FF0000", 2);
            LINE(x, maximumPeak.prev_point, x, maximumPeak.next_point);
          );
          // found an edgel
          add_edgel(x, maximumPeak.point, Edgel::positive);
          begin_found = true;

          maximumPeak.reset();
        }

        if(minimumPeak.add(y, prev_y, next_y, gradient))
        {
          DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_vertical",
            IMAGE_DRAWING_CONTEXT;
            CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
            PEN("00FFFF", 2);
            LINE(x, minimumPeak.prev_point, x, minimumPeak.next_point);
          );
          // found an edgel
          add_edgel(x, minimumPeak.point, Edgel::negative);

          if(begin_found) {
              // found a new double edgel
              add_double_edgel(scan_id, true);
              begin_found = false;
          }
          minimumPeak.reset();
        }

      }
    }
  }
}// end scan_vertical


inline bool ScanGridEdgelDetector::refine_vertical(MaxPeakScan& maximumPeak, int x)
{
  int height = getImage().height();

  // check the range because we need access to a previous and next scan point to calculate the gradient
  int start = std::min(maximumPeak.prev_point, height-2);
  int end = std::max(maximumPeak.next_point, 1);

  maximumPeak.reset();

  for(int y=start; y>=end; --y) {
    // gradient = next - previous
    int gradient = getImage().getY(x, y-1) - getImage().getY(x, y+1);
    // HACK: -1, -1 cause interval isn't of interest here
    if(maximumPeak.add(y, -1, -1, gradient)) {
      // we are just looking for one peak here
      if(!params.full_refinement) {
        break;
      }
    }
  }
  return maximumPeak.found;
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
    if(y <= polyLineLeft.begin().y) {
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
    minimumPeak.reset();
    maximumPeak.reset();

    // fix range
    int start_x = std::max(x, scanline.skip);
    end_x = std::min(end_x, static_cast<int>(getImage().width())-(scanline.skip + 1));

    DEBUG_REQUEST("Vision:ScanGridEdgelDetector:scanlines:horizontal",
      IMAGE_DRAWING_CONTEXT;
      CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
      PEN("FFFFFF", 2);

      LINE(start_x, scanline.y, end_x, scanline.y);

      PEN("FF0000", 2);
      for(int xx = start_x; xx <= end_x; xx += scanline.skip) {
        if(!getBodyContour().isOccupied(xx, scanline.y)) {
          FILLOVAL(xx, scanline.y, 1, 1);
        }
      }
    );

    for(x = start_x; x <= end_x; x += scanline.skip)
    {
      if(scanline.skip > 1) {
        // we are skipping pixels, do an interval scan
        int prev_x = x - scanline.skip;
        int check_x = (prev_x + x)/2;

        // gradient = next - prev
        int gradient = getImage().getY(x, y) - getImage().getY(prev_x, y);

        // begin
        if(maximumPeak.add(check_x, prev_x, x, gradient)) {
          // found local maximum peak
          DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
            IMAGE_DRAWING_CONTEXT;
            CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
            PEN("FF0000", 2);
            LINE(maximumPeak.prev_point, y, maximumPeak.next_point, y);
          );
          if(refine_horizontal(maximumPeak, y)) {
            // don't add edgels that are found in the robots own body
            if(!getBodyContour().isOccupied(maximumPeak.point, y)) {
              // found an edgel
              add_edgel(maximumPeak.point, y, Edgel::positive);
              begin_found = true;
            }
          }
          maximumPeak.reset();
        }

        // end
        if(minimumPeak.add(check_x, prev_x, x, gradient)) {
          // found local minimum peak
          DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
            IMAGE_DRAWING_CONTEXT;
            CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
            PEN("00FFFF", 2);
            LINE(minimumPeak.prev_point, y, minimumPeak.next_point, y);
          );
          if(refine_horizontal(minimumPeak, y)) {
            // don't add edgels that are found in the robots own body
            if(!getBodyContour().isOccupied(minimumPeak.point, y))
            {
              // found an edgel
              add_edgel(minimumPeak.point, y, Edgel::negative);
              // found a new double edgel
              if(begin_found) {
                add_double_edgel(scan_id);
                begin_found = false;
              }
            }

          }
          minimumPeak.reset();
        }
      } else {
        // scan every pixel
        // gradient = next - prev
        int gradient = getImage().getY(x+1, y) - getImage().getY(x-1, y);

        // begin
        if(maximumPeak.add(x, x-1, x+1, gradient)) {
          // found local maximum peak
          DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
            IMAGE_DRAWING_CONTEXT;
            CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
            PEN("FF0000", 2);
            LINE(maximumPeak.prev_point, y, maximumPeak.next_point, y);
          );
          // don't add edgels that are found in the robots own body
          if(!getBodyContour().isOccupied(maximumPeak.point,y))
          {
            // found an edgel
            add_edgel(maximumPeak.point, y, Edgel::positive);
            begin_found = true;
          }
          maximumPeak.reset();
        }

        // end
        if(minimumPeak.add(x, x-1, x+1, gradient)) {
          // found local minimum peak
          DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
            IMAGE_DRAWING_CONTEXT;
            CANVAS(((cameraID==CameraInfo::Top)? "ImageTop": "ImageBottom"));
            PEN("00FFFF", 2);
            LINE(minimumPeak.prev_point, y, minimumPeak.next_point, y);
          );
          // don't add edgels that are found in the robots own body
          if(!getBodyContour().isOccupied(minimumPeak.point, y))
          {
            // found an edgel
            add_edgel(minimumPeak.point, y, Edgel::negative);
            if(begin_found) {
              // found a new double edgel
              add_double_edgel(scan_id);
              begin_found = false;
            }
          }
          minimumPeak.reset();
        }
      }
    }
    ++scan_id;
  }
} // end scan_horizontal


inline bool ScanGridEdgelDetector::refine_horizontal(MaxPeakScan& maximumPeak, int y)
{
  int width = getImage().width();

  // check the range because we need access to a previous and next scan point to calculate the gradient
  int start = std::max(maximumPeak.prev_point, 1);
  int end = std::min(maximumPeak.next_point, width-2);

  maximumPeak.reset();

  for(int x=start; x<=end; ++x) {
    // gradient = next - prev
    int gradient = getImage().getY(x+1, y) - getImage().getY(x-1, y);
    // HACK: -1, -1 cause interval isn't of interest here
    if(maximumPeak.add(x, -1, -1, gradient)) {
      // we are just looking for one peak here
      if(!params.full_refinement) {
        break;
      }
    }
  }
  return maximumPeak.found;
} // end refine_horizontal


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
