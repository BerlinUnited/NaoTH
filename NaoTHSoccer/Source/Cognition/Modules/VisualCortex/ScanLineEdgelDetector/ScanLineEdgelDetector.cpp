/* 
 * File:   ScanLineEdgelDetector.cpp
 * Author: claas
 * Author: Heinrich Mellmann
 * 
 * Created on 14. march 2011, 14:22
 */

#include "ScanLineEdgelDetector.h"

// tools
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/CameraGeometry.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/ImageProcessing/MaximumScan.h"

ScanLineEdgelDetector::ScanLineEdgelDetector()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:ScanLineEdgelDetector:scanlines", "mark the scan lines", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanLineEdgelDetector:mark_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanLineEdgelDetector:mark_double_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:ScanLineEdgelDetector:mark_endpoints", "mark the endpints on the image", false);

  DEBUG_REQUEST_REGISTER("Vision:ScanLineEdgelDetector:mark_scan_segments", "...", false);

  getDebugParameterList().add(&params);
}

ScanLineEdgelDetector::~ScanLineEdgelDetector()
{
  getDebugParameterList().remove(&params);
}

void ScanLineEdgelDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  getScanLineEdgelPercept().reset();

  // TODO: implement a general validation for timestamps
  if(getBodyContour().timestamp != getFrameInfo().getTime()) {
    return;
  }
 
  int horizon_height = (int)(std::min(getArtificialHorizon().begin().y, getArtificialHorizon().end().y)+0.5);
  // clamp it to the image
  horizon_height = Math::clamp(horizon_height,0,(int)getImage().height());

  // scan only inside the estimated field region
  //horizon_height = getFieldPerceptRaw().getValidField().points[0].y;

  int scanline_count = (cameraID ==CameraInfo::Top)? params.scanline_count_top: params.scanline_count_bottom;

  // horizontal stepsize between the scanlines
  double step = static_cast<double>(getImage().width()) / static_cast<double>(scanline_count);
  double scanline_x = step / 2.0;

  // don't scan the lower lines in the image
  int borderY = getImage().height() - params.pixel_border_y - 1;
  
  // start and endpoints for the scanlines
  Vector2i start((int) scanline_x, borderY);
  Vector2i end((int) scanline_x, horizon_height );
  
  for (int i = 0; i < scanline_count; i++)
  {
    ASSERT(getImage().isInside(start.x, start.y));
    // don't scan the own body
    start = getBodyContour().getFirstFreeCell(start);

    // execute the scan
    ScanLineEdgelPercept::EndPoint endPoint = scanForEdgels(i, start, end);
    
    // check if endpoint is not same as the begin of the scanline
    if(endPoint.posInImage == start) {
      endPoint.color = ColorClasses::none;
      endPoint.posInImage.y = borderY;
    }

    // try to project it on the ground
    endPoint.valid = CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(),
      getCameraInfo(),
      endPoint.posInImage.x,
      endPoint.posInImage.y,
      0.0,
      endPoint.posOnField);

    //
    getScanLineEdgelPercept().endPoints.push_back(endPoint);

    scanline_x += step;
    start.y = borderY;
    start.x = (int) (scanline_x + 0.5);
    end.x = start.x;
  }//end for


  DEBUG_REQUEST("Vision:ScanLineEdgelDetector:mark_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];
      ColorClasses::Color color = ColorClasses::black;
      if(edgel.type == Edgel::positive) {
        color = ColorClasses::blue;
      } else if(edgel.type == Edgel::negative) {
        color = ColorClasses::red;
      }
      LINE_PX(color,edgel.point.x, edgel.point.y, edgel.point.x + (int)(edgel.direction.x*10), edgel.point.y + (int)(edgel.direction.y*10));
    }
  );

  // mark finished valid edgels
  DEBUG_REQUEST("Vision:ScanLineEdgelDetector:mark_double_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPercept().pairs[i];
      CIRCLE_PX(ColorClasses::black, (int)pair.point.x, (int)pair.point.y, 3);
      LINE_PX(ColorClasses::red   ,(int)pair.point.x, (int)pair.point.y, (int)(pair.point.x + pair.direction.x*10), (int)(pair.point.y + pair.direction.y*10));
    }
  );
  
  DEBUG_REQUEST("Vision:ScanLineEdgelDetector:mark_endpoints",
    for(size_t i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
    {
      const ScanLineEdgelPercept::EndPoint& point_two = getScanLineEdgelPercept().endPoints[i];
      CIRCLE_PX(point_two.color, point_two.posInImage.x, point_two.posInImage.y, 5);

      if(i > 0) {
        const ScanLineEdgelPercept::EndPoint& point_one = getScanLineEdgelPercept().endPoints[i-1];
        LINE_PX(point_one.color,
                point_one.posInImage.x, point_one.posInImage.y,
                point_two.posInImage.x, point_two.posInImage.y);
      }

      if(!point_two.greenFound) {
        CIRCLE_PX(ColorClasses::gray, point_two.posInImage.x, point_two.posInImage.y, 7);
      }
    }
  );
}//end execute


ScanLineEdgelPercept::EndPoint ScanLineEdgelDetector::scanForEdgels(int scan_id, const Vector2i& start, const Vector2i& end)
{
  ScanLineEdgelPercept::EndPoint endPoint;
  endPoint.posInImage = start;
  endPoint.color = ColorClasses::none;
  endPoint.ScanLineID = scan_id;

  const int step = 2;

  if(end.y + step > start.y || end.y + step >= (int)getImage().height()) {
    endPoint.posInImage.y = end.y;
    return endPoint;
  }


  // no scan if the start is at the top of the image
  if(start.y <= step) {
    return endPoint;
  }

  Vector2i point(start);
  // we have no idea what this should be good for, but for now it makes tons of problems
  //point.y -= step; // make one step

  //Vector2i last_down_point(point); // needed for the endpoint
  bool begin_found = false;

  // calculate the threashold
  int t_edge = params.brightness_threshold_top;
  // HACK (TEST): make it dependend on the angle of the camera in the future
  if(cameraID == CameraInfo::Bottom) {
    t_edge = params.brightness_threshold_bottom;
  }

  // calculate the threshold depending on the reprojected size of the ball in the image
  if(params.dynamicThreshold)
  {
    double radius = CameraGeometry::estimatedBallRadius(
      getCameraMatrix(),getCameraInfo(), getFieldInfo().ballRadius, 
      getCameraInfo().resolutionWidth / 2, getCameraInfo().resolutionHeight / 4*3);

    t_edge = Math::clamp((int)radius, params.dynamicThresholdMin, params.dynamicThresholdMax);
  }

  Vector2i lastGreenPoint(point); // HACK
  RingBufferWithSum<double, 10> movingWindow; // HACK

  // initialize the scanner
  Vector2i peak_point_max(point);
  Vector2i peak_point_min(point);
  MaximumScan<int,int> positiveScan(peak_point_max.y, t_edge);
  MaximumScan<int,int> negativeScan(peak_point_min.y, t_edge);

  Pixel pixel;
  /*
  const int yRowStepOffset = getImage().width()*step/2;
  const Pixel* imgPtr = reinterpret_cast<Pixel*>(getImage().data());
  imgPtr += point.x / 2;
  imgPtr += point.y * getImage().width()/2;
  */
  int numberOfGreen = 0; 
  int numberOfSamples = 0;

  int f_last = getImage().getY(point.x, point.y); // scan the first point
  // just go up
  for(;point.y >= end.y + step; point.y -= step)
  {
    // get the brightness chanel
    getImage().get_direct(point.x, point.y, pixel);
    //const Pixel& pixel = getImage().getRef(point.x, point.y);

    //const Pixel& pixel = *imgPtr;

    //int f_y = getImage().getY(point.x, point.y);
    int f_y = pixel.y;
    int g = f_y - f_last;
    f_last = f_y;

    // begin found
    if(positiveScan.add(point.y+1, g))
    {
      // refine the position of the peak
      int f_2 = getImage().getY_direct(point.x, peak_point_max.y-2);
      int f0  = getImage().getY_direct(point.x, peak_point_max.y);
      int f2  = getImage().getY_direct(point.x, peak_point_max.y+2);

      if(f_2-f0 > positiveScan.maxValue()) peak_point_max.y -= 1;
      if(f0 -f2 > positiveScan.maxValue()) peak_point_max.y += 1;

      // add new up edgel
      add_edgel(peak_point_max, Edgel::positive);
      begin_found = true;

      //last_down_point.y = peak_point_max.y;

      numberOfGreen = 0; 
      numberOfSamples = 0;
    }//end if

    // end found
    if(negativeScan.add(point.y+1, -g))
    {
      // refine the position of the peak
      int f_2 = getImage().getY_direct(point.x, peak_point_min.y-2);
      int f0  = getImage().getY_direct(point.x, peak_point_min.y);
      int f2  = getImage().getY_direct(point.x, peak_point_min.y+2);
        
      if(f_2-f0 < negativeScan.maxValue()) peak_point_min.y -= 1;
      if(f0 -f2 < negativeScan.maxValue()) peak_point_min.y += 1;

      // add new down edgel
      add_edgel(peak_point_min, Edgel::negative);
      
      // new end edgel
      // found a new double edgel
      bool not_in_green_area = (numberOfSamples <= 3 || numberOfGreen*2 < numberOfSamples);
      if(begin_found && (!params.double_edgel_green_check || not_in_green_area)) {
        add_double_edgel(scan_id);
      }

      begin_found = false;


      //last_down_point.y = peak_point_min.y;

      numberOfGreen = 0; 
      numberOfSamples = 0;
    }//end if


    // HACK
    if(getFieldColorPercept().isFieldColor(pixel))
    {
      if(movingWindow.getAverage() > params.minEndPointGreenDensity)
      {
        lastGreenPoint = point;
        endPoint.greenFound = true;
      }
      movingWindow.add(1.0);
      numberOfGreen++;
    }
    else
    {
      //HACK break if darker than field
      /*
      if(pixel.y < getFieldColorPercept().range.getMin().y)
      {
        break;
      }
      */
      movingWindow.add(0.0);
    }

    DEBUG_REQUEST("Vision:ScanLineEdgelDetector:scanlines",
      //Pixel pixel = getImage().get(point.x, point.y);
      ColorClasses::Color thisPixelColor = (getFieldColorPercept().isFieldColor(pixel))?ColorClasses::green:ColorClasses::none;
      POINT_PX(thisPixelColor, point.x, point.y);
    );

    numberOfSamples++;
    //imgPtr -= yRowStepOffset;
  }//end for

  /*
  if(point.y < end.y + step) {
    if(estimateColorOfSegment(last_down_point, end) == ColorClasses::green) {
        endPoint.color = ColorClasses::green;
        endPoint.posInImage = end;
      }
  }*/
  endPoint.posInImage = lastGreenPoint;
  endPoint.color = ColorClasses::green;

  return endPoint;
}//end scanForEdgels


bool ScanLineEdgelDetector::validDistance(const Vector2i& pointOne, const Vector2i& pointTwo) const
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


ColorClasses::Color ScanLineEdgelDetector::estimateColorOfSegment(const Vector2i& begin, const Vector2i& end) const
{
  ASSERT(begin.x == end.x);

  // switch if necessary
  int beginY = begin.y;
  int endY = end.y;
  if(end.y > begin.y) {
    beginY = end.y;
    endY = begin.y;
  }

  const int numberOfSamples = params.green_sampling_points;
  int length = beginY - endY; //begin.y - end.y;
  int numberOfGreen = 0;
  Vector2i point(begin.x, beginY);
  Pixel pixel;

  if(numberOfSamples >= length) 
  {
    for(; point.y > endY; --point.y)
    {
      getImage().get(point.x, point.y, pixel);
      numberOfGreen += getFieldColorPercept().isFieldColor(pixel);
    }
  }
  else
  {
    int step = length / numberOfSamples;
    int offset = Math::random(length); // number in [0,length-1]

    for(int i = 0; i < numberOfSamples; ++i)
    {
      int k = (offset + i*step) % length;
      point.y = endY + k;
      getImage().get(point.x, point.y, pixel);
      numberOfGreen += getFieldColorPercept().isFieldColor(pixel);
    }
  }

  ColorClasses::Color c = (numberOfGreen > numberOfSamples/2) ? ColorClasses::green : ColorClasses::none;

  DEBUG_REQUEST("Vision:ScanLineEdgelDetector:mark_scan_segments",
    LINE_PX(c, begin.x, begin.y, end.x, end.y);
  );

  return c;
}//end estimateColorOfSegment


Vector2d ScanLineEdgelDetector::calculateGradient(const Vector2i& point) const
{
  Vector2d gradient;
  static const int offset = 1;

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
}//end calculateGradient

