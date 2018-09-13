#include "ScanGridEdgelDetector.h"

// tools
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/CameraGeometry.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

ScanGridEdgelDetector::ScanGridEdgelDetector()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:scanlines", "mark the scan lines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_double_edgels", "mark the edgels on the image", false);

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

  //TODO debug request
  /*
  int x = 320;
  for(size_t i=1; i<getScanGrid().yScanPattern.size()-1; ++i) {
    int y = getScanGrid().yScanPattern[i];
    int prev = getImage().getY(x, getScanGrid().yScanPattern[i-1]);
    int curr = getImage().getY(x, getScanGrid().yScanPattern[i]);
    double gradient = .5*(curr - prev);

    PLOT_GENERIC(cameraID==CameraInfo::Top?"VScanLineTop":"VScanLineBottom", static_cast<double>(i), getImage().getY(x, y));
    PLOT_GENERIC(cameraID==CameraInfo::Top?"GradiantTop":"GradiantBottom", static_cast<double>(i), gradient);

    POINT_PX(ColorClasses::blue, x, y);
  }
  */

  // threshold
  int t_edge = cameraID == CameraInfo::Top? parameters.brightness_threshold_top : parameters.brightness_threshold_bottom;

  // initialize the scanner
  MaximumScan<int,int> positiveScan(0, t_edge);
  MinimumScan<int,int> negativeScan(0, t_edge);

  MaximumScan<int,int> refinedMaximumScan(0, t_edge);
  MinimumScan<int,int> refinedMinimumScan(0, t_edge);
  int x, y, luma, lastLuma, gradient, last_y;
  int scan_id = 0;
  for(const ScanGrid::VScanLine scanline: getScanGrid().vertical)
  {
    bool begin_found = false;
    x = scanline.x;
    y = getScanGrid().vScanPattern[scanline.bottom];

    positiveScan.clean(y);
    negativeScan.clean(y);

    lastLuma = getImage().getY(scanline.x, y);
    last_y = getScanGrid().vScanPattern[scanline.bottom];

    int end_of_field = getFieldPercept().getValidField().getClosestPoint(Vector2i(x, getScanGrid().vScanPattern[scanline.top])).y;

    for(size_t i=scanline.bottom; i>scanline.top; --i) {
      y = getScanGrid().vScanPattern[i];
      if(y < end_of_field) {
        break;
      }
      luma = getImage().getY(x, y);
      gradient = luma - lastLuma;
      lastLuma = luma;

      // begin found
      if(positiveScan.add(last_y, gradient))
      {
        int startY = getScanGrid().vScanPattern[i+2];
        refinedMaximumScan.clean(startY);
        refine_edge(refinedMaximumScan, x, startY, last_y);
        int refinedY = refinedMaximumScan.peakPoint;
        add_edgel(x, refinedY);
        begin_found = true;
      }

      // end found
      if(negativeScan.add(last_y, gradient))
      {
        int startY = getScanGrid().vScanPattern[i+2];
        refinedMinimumScan.clean(startY);
        refine_edge(refinedMinimumScan, x, startY, last_y);
        int refinedY = refinedMinimumScan.peakPoint;
        add_edgel(x, refinedY);

        // new end edgel
        // found a new double edgel
        if(begin_found) {
          add_double_edgel(scan_id);
        }
        begin_found = false;
      }

      last_y = y;
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


inline void ScanGridEdgelDetector::refine_edge(MaximumScan<int,int>& refinedScan, int x, int start_y, int end_y){
  int luma, gradient;
  int lastLuma = getImage().getY(x, start_y);
  for(int y=start_y-1; y>=end_y-2 && y>=0; --y) {
    luma = getImage().getY(x, y);
    gradient = luma - lastLuma;
    lastLuma = luma;
    if(refinedScan.add(y+1, gradient)) {
      return;
    }
  }
  return;
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
