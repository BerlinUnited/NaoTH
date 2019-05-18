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
  int t_edge =
      cameraID == CameraInfo::Top? parameters.brightness_threshold_top:
                                   parameters.brightness_threshold_bottom;

  // initialize the scanner
  MaxPeakScan maximumPeak(t_edge);
  MinPeakScan minimumPeak(t_edge);

  int x, y, luma, prevLuma, gradient, prevPoint;
  bool begin_found;

  // vertical scanlines
  int scan_id = 0;
  for(const ScanGrid::VScanLine scanline: getScanGrid().vertical)
  {
    begin_found = false;
    x = scanline.x;
    y = getScanGrid().vScanPattern[scanline.bottom];

    // TODO: set end of field to field area
    int end_of_field = 10;

    prevLuma = getImage().getY(scanline.x, y);

    int end_of_body =
        getBodyContour().getFirstFreeCell(Vector2i(scanline.x, y)).y;

    prevPoint = end_of_body;

    for(size_t i = scanline.bottom; i > scanline.top; --i) {
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
        if(refine_vertical(maximumPeak, x)) {
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
        if(refine_vertical(minimumPeak, x)) {
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

  maximumPeak.reset();
  minimumPeak.reset();

  // horizontal scanlines
  for(const ScanGrid::HScanLine scanline: getScanGrid().horizontal)
  {
    begin_found = false;
    x = scanline.left_x;
    y = scanline.y;

    prevLuma = getImage().getY(x, y);
    prevPoint = x;
    for(;x <= scanline.right_x; x += scanline.skip) {

      luma = getImage().getY(x, y);
      gradient = luma - prevLuma;

      // begin
      if(maximumPeak.add(x, prevPoint, gradient)) {
        // found greatest peak
        DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_jump_horizontal",
          LINE_PX(ColorClasses::red, maximumPeak.prev_point, y, maximumPeak.point, y);
        );
        if(refine_horizontal(maximumPeak, y) && !getBodyContour().isOccupied(maximumPeak.point,y)) {
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
        if(refine_horizontal(minimumPeak, y) && !getBodyContour().isOccupied(minimumPeak.point,y)) {
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

  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];
      LINE_PX(ColorClasses::black,edgel.point.x, edgel.point.y, edgel.point.x + (int)(edgel.direction.x*10), edgel.point.y + (int)(edgel.direction.y*10));
    }
  );
  // mark finished valid edgels
  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_double_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPercept().pairs[i];
      CIRCLE_PX(ColorClasses::black, (int)pair.point.x, (int)pair.point.y, 3);
      LINE_PX(ColorClasses::red   ,(int)pair.point.x, (int)pair.point.y, (int)(pair.point.x + pair.direction.x*10), (int)(pair.point.y + pair.direction.y*10));
    }
  );
}//end execute

inline bool ScanGridEdgelDetector::refine_vertical(MaxPeakScan& maximumPeak, int x) {
  int height = getImage().height();
  int start = std::min(maximumPeak.prev_point, height-2);
  int end = std::max(maximumPeak.point-1, 0);
  maximumPeak.reset();

  int prev = std::min(start+2, height-1);
  int luma, gradient;
  int prevLuma = getImage().getY(x, prev);

  int f0, f_2, f2;

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
    // refine the position of the peak
    f0 = getImage().getY(x, maximumPeak.point);
    if (maximumPeak.point+2 < height) {
      f2 = getImage().getY(x, maximumPeak.point+2);
      maximumPeak.check(maximumPeak.point+1, f0-f2);
    }
    if(maximumPeak.point-2 >= 0) {
      f_2 = getImage().getY(x, maximumPeak.point-2);
      maximumPeak.check(maximumPeak.point-1, f_2-f0);
    }
    return true;
  }
  return false;
}

inline bool ScanGridEdgelDetector::refine_horizontal(MaxPeakScan& maximumPeak, int y) {
  int width = getImage().width();
  int start = std::max(maximumPeak.prev_point, 1);
  int end = std::min(maximumPeak.point+1, width-1);
  maximumPeak.reset();

  int prev = std::max(start-2, 0);
  int luma, gradient;
  int prevLuma = getImage().getY(prev, y);

  int f0, f_2, f2;

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
    // refine the position of the peak
    f0 = getImage().getY(maximumPeak.point, y);
    if(maximumPeak.point-2 >= 0) {
      f_2 = getImage().getY(maximumPeak.point-2, y);
      maximumPeak.check(maximumPeak.point-1, f_2-f0);
    }
    if(maximumPeak.point+2 < width) {
      f2 = getImage().getY(maximumPeak.point+2, y);
      maximumPeak.check(maximumPeak.point+1, f0-f2);
    }
    return true;
  }
  return false;
}

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
}//end calculateGradient
