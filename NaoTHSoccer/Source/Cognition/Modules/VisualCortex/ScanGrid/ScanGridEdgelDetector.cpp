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
  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_endpoints", "mark the endpints on the image", false);

  DEBUG_REQUEST_REGISTER("Vision:ScanGridEdgelDetector:mark_scan_segments", "...", false);

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
  for(const ScanGrid::VScanLine scanline: getScanGrid().vertical)
  {
    x = scanline.x;
    y = getScanGrid().vScanPattern[scanline.bottom];

    positiveScan.clean(y);
    negativeScan.clean(y);

    lastLuma = getImage().getY(scanline.x, y);
    last_y = getScanGrid().vScanPattern[scanline.bottom];

    for(size_t i=scanline.bottom; i>scanline.top; --i) {
      y = getScanGrid().vScanPattern[i];
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
      }

      // end found
      if(negativeScan.add(last_y, gradient))
      {
        int startY = getScanGrid().vScanPattern[i+2];
        refinedMinimumScan.clean(startY);
        refine_edge(refinedMinimumScan, x, startY, last_y);
        int refinedY = refinedMinimumScan.peakPoint;
        add_edgel(x, refinedY);
      }

      last_y = y;
    }
  }

  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];
      LINE_PX(ColorClasses::black,edgel.point.x, edgel.point.y, edgel.point.x + (int)(edgel.direction.x*10), edgel.point.y + (int)(edgel.direction.y*10));
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

bool ScanGridEdgelDetector::validDistance(const Vector2i& pointOne, const Vector2i& pointTwo) const
{
  if (abs(pointOne.y - pointTwo.y) > 5)
  {
    Vector2d beginOnTheGround;
    if(CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(),
      getCameraInfo(),
      pointOne.x,
      pointOne.y,
      0.0,
      beginOnTheGround))
    {
      Vector2d endOnTheGround;
      if(CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getCameraInfo(),
        pointTwo.x,
        pointTwo.y,
        0.0,
        endOnTheGround))
      {
        if((beginOnTheGround-endOnTheGround).abs2() > 700*700) {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  return true;
}


ColorClasses::Color ScanGridEdgelDetector::estimateColorOfSegment(const Vector2i& begin, const Vector2i& end) const
{
  ASSERT(begin.x == end.x && end.y <= begin.y);

  const int numberOfSamples = parameters.green_sampling_points;
  int length = begin.y - end.y;
  int numberOfGreen = 0;
  Vector2i point(begin);
  Pixel pixel;

  if(numberOfSamples >= length)
  {
    for(; point.y > end.y; point.y--)
    {
      getImage().get(point.x, point.y, pixel);
      numberOfGreen += getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
    }
  }
  else
  {
    int step = length / numberOfSamples;
    int offset = Math::random(length); // number in [0,length-1]

    for(int i = 0; i < numberOfSamples; i++)
    {
      int k = (offset + i*step) % length;
      point.y = end.y + k;
      getImage().get(point.x, point.y, pixel);
      numberOfGreen += getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
    }
  }

  ColorClasses::Color c = (numberOfGreen > numberOfSamples/2) ? ColorClasses::green : ColorClasses::none;

  DEBUG_REQUEST("Vision:ScanGridEdgelDetector:mark_scan_segments",
    LINE_PX(c, begin.x, begin.y, end.x, end.y);
  );

  return c;
}//end estimateColorOfSegment


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
