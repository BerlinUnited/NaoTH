/* 
 * File:   ScanLineEdgelDetector.cpp
 * Author: claas
 * Author: Heinrich Mellmann
 * 
 * Created on 14. march 2011, 14:22
 */

#include "ScanLineEdgelDetector.h"

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"

// tools
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/CameraGeometry.h"

ScanLineEdgelDetector::ScanLineEdgelDetector()
:
  cameraID(CameraInfo::Top)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetector:mark_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetector:mark_double_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetector:mark_endpoints", "mark the endpints on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetector:scanlines", "mark the scan lines", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetector:expected_line_width", "", false);
}

ScanLineEdgelDetector::~ScanLineEdgelDetector()
{}

void ScanLineEdgelDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);
  getScanLineEdgelPercept().reset();

  // needs a valid field polygon
  //if(!getFieldPerceptRaw().valid) {
  //  return;
  //}
  // TODO: implement a general validation for timestamps
  if(getBodyContour().timestamp != getFrameInfo().getTime()) {
    return;
  }
 
  int horizon_height = (int)(std::min(getArtificialHorizon().begin().y, getArtificialHorizon().end().y)+0.5);
  // clamp it to the image
  horizon_height = Math::clamp(horizon_height,0,(int)getImage().height());

  // scan only inside the estimated field region
  //horizon_height = getFieldPerceptRaw().getValidField().points[0].y;

  // horizontal stepsize between the scanlines
  int step = (getImage().width() - 1) / (theParameters.scanline_count - 1);

  // don't scan the lower lines in the image
  int borderY = getImage().height() - theParameters.pixel_border_y - 1;
  
  // start and endpoints for the scanlines
  Vector2i start(step / 2, borderY);
  Vector2i end(step / 2, horizon_height );
  
  for (int i = 0; i < theParameters.scanline_count - 1; i++)
  {
    ASSERT(getImage().isInside(start.x, start.y));
    // don't scan the own body
    start = getBodyContour().getFirstFreeCell(start);

    // execute the scan
    ScanLineEdgelPercept::EndPoint endPoint = scanForEdgels(i, start, end);
    
    // check if endpoint is not same as the begin of the scanline
    if(endPoint.posInImage.y == start.y) {
      endPoint.color = ColorClasses::none;
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

    start.y = borderY;
    start.x += step;
    end.x = start.x;
  }//end for


  DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetector:mark_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];
      Vector2d g1 = Vector2d(10.0,0).rotate(edgel.angle).rotateRight();
      LINE_PX(ColorClasses::black,edgel.point.x, edgel.point.y, edgel.point.x + (int)(g1.x), edgel.point.y + (int)(g1.y));
    }
  );

  // mark finished valid edgels
  DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetector:mark_double_edgels",
    for(size_t i = 0; i < getScanLineEdgelPercept().scanLineEdgels.size(); i++)
    {
      const DoubleEdgel& double_edgel = getScanLineEdgelPercept().scanLineEdgels[i];
      LINE_PX(ColorClasses::red   ,double_edgel.begin.x ,double_edgel.begin.y ,double_edgel.begin.x + (int)(10 * cos(double_edgel.begin_angle))   ,double_edgel.begin.y + (int)(10 * sin(double_edgel.begin_angle)));
      LINE_PX(ColorClasses::blue  ,double_edgel.center.x,double_edgel.center.y,double_edgel.center.x + (int)(10 * cos(double_edgel.center_angle)) ,double_edgel.center.y + (int)(10 * sin(double_edgel.center_angle)));
      LINE_PX(ColorClasses::black ,double_edgel.end.x   ,double_edgel.end.y   ,double_edgel.end.x + (int)(10 * cos(double_edgel.end_angle))       ,double_edgel.end.y + (int)(10 * sin(double_edgel.end_angle)));
    }
  );
  
  DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetector:mark_endpoints",
    for(size_t i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
    {
      const ScanLineEdgelPercept::EndPoint& point = getScanLineEdgelPercept().endPoints[i];
      
      CIRCLE_PX(point.color, point.posInImage.x, point.posInImage.y, 5);
      if(i > 0)
      {
        const ScanLineEdgelPercept::EndPoint& last_point = getScanLineEdgelPercept().endPoints[i-1];
        LINE_PX(last_point.color,
                last_point.posInImage.x, last_point.posInImage.y,
                point.posInImage.x, point.posInImage.y);
      }//end if
    }//end for
  );
}//end execute


// scanForEdgels ala HTWK
// TODO: reference to the master thesis von Thomas Reinhard
ScanLineEdgelPercept::EndPoint ScanLineEdgelDetector::scanForEdgels(int scan_id, const Vector2i& start, const Vector2i& end)
{
  //results
  DoubleEdgel double_edgel;
  //Edgel edgel;

  Vector2i point(start);
  point.y -= 2; // make one step

  Vector2i lastGreenPoint(start); // needed for the endpoint
  Vector2i peak_point(start);

  // initialize
  int t_edge = theParameters.brightness_threshold * 2;
  int g_min = t_edge;
  int g_max = -t_edge;
  int& x_peak = peak_point.y;//0;
  int f_last = getImage().getY(point.x, point.y);

  //
  //double greenCount = 0;
  //double segmentLength = 0;

  bool begin_found = false;
  int begin_y = 0;

  // just go up
  for(;point.y >= end.y; point.y -= 2)
  {
    // get the pixel color
    //Pixel pixel = getImage().get(point.x, x_peak);
    //bool green = getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
    //if(green) greenCount++;
    //segmentLength++;

    // get the brightness chanel
    int f_x = getImage().getY(point.x, point.y);
    int g = f_x - f_last;
    
    if(g > g_max)
    {
      /*
      DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetector:mark_edgels",
        POINT_PX(ColorClasses::pink, start.x, x_peak);
      );
      */

      if(g_min < -t_edge)// end found
      {

        //if( greenCount/segmentLength > 0.3 ) {
        if(estimateColorOfSegment(lastGreenPoint, peak_point) == ColorClasses::green) {
          lastGreenPoint.y = x_peak;
        }

        //begin_of_segment = x_peak;
        //greenCount = 0;
        //segmentLength = 1;

        // new end edgel
        add_edgel(start.x, x_peak); // peak_point

        if(begin_found) // secure edgel
        {
          DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetector:mark_double_edgels",
            POINT_PX(ColorClasses::red, start.x, x_peak);
          );
          begin_found = false;

          //a new double edgel
          add_edgel(start.x, begin_y, start.x, x_peak, scan_id);
        }
        else
        {
          DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetector:mark_double_edgels",
            POINT_PX(ColorClasses::yellow, start.x, x_peak);
          );
        }
      }//end if

      g_max = g;
      g_min = t_edge;
      x_peak = point.y+1; // +0.5?
    }// end if

    
    if(g < g_min)
    {
      if(g_max > t_edge)// begin found
      {
        DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetector:mark_double_edgels",
          POINT_PX(ColorClasses::blue, start.x, (int)x_peak);
        );

        // new begin edgel
        add_edgel(start.x, x_peak); // peak_point

        //if( greenCount/segmentLength > 0.3 ) {
        if(estimateColorOfSegment(lastGreenPoint, peak_point) == ColorClasses::green) {
          lastGreenPoint.y = x_peak;
        }

        //greenCount = 0;
        //segmentLength = 1;
        //begin_of_segment = x_peak;
        begin_found = true;
        begin_y = x_peak;
      }//end if

      g_max = -t_edge;
      g_min = g;
      x_peak = point.y+1; // +0.5?
    }//end if

    f_last = f_x;

    DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetector:scanlines",
      Pixel pixel = getImage().get(point.x, point.y);
      ColorClasses::Color thisPixelColor = (getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c))?ColorClasses::green:ColorClasses::none;
      POINT_PX(thisPixelColor, point.x, point.y);
    );
  }//end for

  // check the color of the last segment
  //if(greenCount/segmentLength > 0.3) {
  if(estimateColorOfSegment(lastGreenPoint, peak_point) == ColorClasses::green) {
    lastGreenPoint.y = x_peak;
  }

  ScanLineEdgelPercept::EndPoint endPoint;
  endPoint.posInImage = lastGreenPoint;
  endPoint.color = ColorClasses::green;
  endPoint.ScanLineID = scan_id;

  return endPoint;
}//end scanForEdgels


ColorClasses::Color ScanLineEdgelDetector::estimateColorOfSegment(const Vector2i& begin, const Vector2i& end) const
{
  ASSERT(begin.x == end.x && end.y <= begin.y);

  int length = begin.y - end.y;
  int numberOfGreen = 0;
  const int numberOfSamples = std::min(length, 20);
  Vector2i point(begin);
  Pixel pixel;

  for(int i = 0; i < numberOfSamples; i++)
  {
    int k = Math::random(length+1); // number in [0,length]
    point.y = end.y + k;
    getImage().get(point.x, point.y, pixel);
    numberOfGreen += getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
  }//end for

  return (numberOfGreen > numberOfSamples/2) ? ColorClasses::green : ColorClasses::none;
}//end estimateColorOfSegment


Vector2d ScanLineEdgelDetector::calculateGradient(const Vector2i& point) const
{
  Vector2d gradient;

  // no angle at the border (shouldn't happen)
  if( point.x < 1 || point.x + 2 > (int)getImage().width() ||
      point.y < 1 || point.y + 2 > (int)getImage().height() ) {
    return gradient;
  }

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means
  
  gradient.x =
       getImage().getY(point.x-1, point.y+1)
    +2*getImage().getY(point.x  , point.y+1)
    +  getImage().getY(point.x+1, point.y+1)
    -  getImage().getY(point.x-1, point.y-1)
    -2*getImage().getY(point.x  , point.y-1)
    -  getImage().getY(point.x+1, point.y-1);

  gradient.y =
       getImage().getY(point.x-1, point.y-1)
    +2*getImage().getY(point.x-1, point.y  )
    +  getImage().getY(point.x-1, point.y+1)
    -  getImage().getY(point.x+1, point.y-1)
    -2*getImage().getY(point.x+1, point.y  )
    -  getImage().getY(point.x+1, point.y+1);

  //calculate the angle of the gradient
  return gradient;
}//end calculateGradient
