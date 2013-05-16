/* 
 * File:   ScanLineEdgelDetectorDifferential.cpp
 * Author: claas
 * Author: Heinrich Mellmann
 * 
 * Created on 14. march 2011, 14:22
 */

#include "ScanLineEdgelDetectorDifferential.h"

#include "Tools/CameraGeometry.h"

ScanLineEdgelDetectorDifferential::ScanLineEdgelDetectorDifferential()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetectorDifferential:scanlines", "mark the scan lines", false);
}


ScanLineEdgelDetectorDifferential::~ScanLineEdgelDetectorDifferential()
{
}


void ScanLineEdgelDetectorDifferential::execute()
{
  // reset the percept
  getScanLineEdgelPercept().reset();


  // scan only inside the estimated field region
  Vector2<unsigned int> beginField = getFieldPercept().getLargestValidRect(getArtificialHorizon()).points[0];

  // horizontal stepsize between the scanlines
  int step = (getImage().cameraInfo.resolutionWidth - 1) / (theParameters.scanline_count - 1);
  //int remaining_pixels = (getImage().cameraInfo.resolutionWidth - 1) % theParameters.scanline_count;

  // don't scan the lower lines in the image
  int borderY = getImage().cameraInfo.resolutionHeight - theParameters.pixel_border_y - 1;
  
  // start and endpoints for the scanlines
  Vector2<int> start(step / 2, borderY);
  Vector2<int> end(step / 2, beginField.y );
  

  for (int i = 0; i < theParameters.scanline_count; i++)
  {
    ASSERT(getImage().isInside(start.x, start.y));
    // don't scan the own body
    start = getBodyContour().returnFirstFreeCell(start);

    // vertical scanlines
    end.x = start.x;

    // execute the scan
    ScanLineEdgelPercept::EndPoint endPoint = scanForEdgels(i, start, end);
    
    
    // check if endpoint is not same as the begin of the scanline
    if(endPoint.posInImage.y >= borderY)
    {
      endPoint.color = ColorClasses::none;
    }//end if


    // try to project it on the ground
    endPoint.valid = CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(),
      getImage().cameraInfo,
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



  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
    for(unsigned int i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
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
ScanLineEdgelPercept::EndPoint ScanLineEdgelDetectorDifferential::scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end) const
{
  Vector2<int> point(start);
  Vector2<int> lastGreenPoint(start); // needed for the endpoint

  //results
  Edgel edgel;

  //Pixel pixel = getImage().get(point.x, point.y);

  // initialize
  int t_edge = theParameters.brightness_threshold * 2;
  int g_min = t_edge;
  int g_max = -t_edge;
  int x_peak = 0;
  int f_last = getImage().getY(point.x, point.y);


  //
  double greenCount = 0;
  double segmentLength = 0;

  bool begin_found = false;
  int begin_y = 0;

//  int begin_of_segment = point.y;

  // just go up
  for(;point.y >= end.y; point.y--)
  {
    // get the pixel color
    Pixel pixel = getImage().get(point.x, x_peak);
    bool green = getColorClassificationModel().getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
    if(green) greenCount++;
    segmentLength++;

    // get the brightness chanel
    int f_x = getImage().getY(point.x, point.y);
    int g = f_x - f_last;
    
    
    if(g > g_max)
    {
      DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
        POINT_PX(ColorClasses::pink, start.x, x_peak);
      );

      if(g_min < -t_edge)// end found
      {

        if( greenCount/segmentLength > 0.3 )
        {
          lastGreenPoint.y = x_peak;
        }

        //begin_of_segment = x_peak;
        greenCount = 0;
        segmentLength = 1;

        if(begin_found) // secure edgel
        {
          DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
            POINT_PX(ColorClasses::red, start.x, x_peak);
          );
          begin_found = false;

 
          //a new edgel
          edgel.begin.x = start.x;
          edgel.begin.y = begin_y;
          edgel.begin_angle = getPointsAngle(edgel.begin);
          /*if(edgel.begin.y > 0)
          {
            double a = getPointsAngle(Vector2<int>(edgel.begin.x, edgel.begin.y-1));
            edgel.begin_angle = calculateMeanAngle(edgel.begin_angle, a);
          }*/

          edgel.end.x = start.x;
          edgel.end.y = x_peak;
          edgel.end_angle = Math::normalizeAngle(getPointsAngle(edgel.end) + Math::pi);
          /*if(edgel.end.y > 0)
          {
            double a = getPointsAngle(Vector2<int>(edgel.end.x, edgel.end.y-1));
            edgel.end_angle = calculateMeanAngle(edgel.end_angle, a);
          }*/

          edgel.center = (edgel.end + edgel.begin) / 2;
          edgel.center_angle = calculateMeanAngle(edgel.begin_angle, edgel.end_angle);
          edgel.valid = true;

          if(edgel.valid)
          {
            edgel.ScanLineID = scan_id;
            getScanLineEdgelPercept().add(edgel);

            // mark finished valid edgels
            DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
              LINE_PX(ColorClasses::black ,edgel.center.x,edgel.center.y,edgel.center.x + (int)(10 * cos(edgel.center_angle)) ,edgel.center.y + (int)(10 * sin(edgel.center_angle)));
            );
          }

        }
        else
        {
          DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
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
        DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
          POINT_PX(ColorClasses::blue, start.x, (int)x_peak);
        );

        if( greenCount/segmentLength > 0.3 )
        {
          lastGreenPoint.y = x_peak;
        }

        greenCount = 0;
        segmentLength = 1;
        //begin_of_segment = x_peak;
        begin_found = true;
        begin_y = x_peak;
      }//end if

      g_max = -t_edge;
      g_min = g;
      x_peak = point.y+1; // +0.5?
    }//end if

    f_last = f_x;


    DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:scanlines",
      Pixel pixel = getImage().get(point.x, point.y);
      ColorClasses::Color thisPixelColor = (getColorClassificationModel().getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c))?ColorClasses::green:ColorClasses::none;
      POINT_PX(thisPixelColor, point.x, point.y);
    );
  }//end for


  // check the color of the last segment
  if(greenCount/segmentLength > 0.3)
  {
    lastGreenPoint.y = x_peak;
  }

  ScanLineEdgelPercept::EndPoint endPoint;
  endPoint.posInImage = lastGreenPoint;
  endPoint.color = ColorClasses::green;
  endPoint.ScanLineID = scan_id;

  return endPoint;
}//end scanForEdgels


ColorClasses::Color ScanLineEdgelDetectorDifferential::estimateColorOfSegment(const Vector2<int>& begin, const Vector2<int>& end) const
{
  ASSERT(begin.x == end.x && end.y <= begin.y);

  int length = begin.y - end.y;
  int numberOfGreen = 0;
  const int numberOfSamples = min(length, 20);
  Vector2<int> point(begin);
  Pixel pixel;

  int meanY = 0;

  for(int i = 0; i < numberOfSamples; i++)
  {
    int k = Math::random(length+1); // number in [0,length]
    point.y = end.y + k;
    getImage().get(point.x, point.y, pixel);
    numberOfGreen += getColorClassificationModel().getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
    meanY += pixel.y;
  }//end for

  if(length > 0) meanY /= length;

  return (numberOfGreen > numberOfSamples/2) ? ColorClasses::green : ColorClasses::none;
}//end estimateColorOfSegment


double ScanLineEdgelDetectorDifferential::getPointsAngle(const Vector2<int>& point) const
{
  // no angle at the border (shouldn't happen)
  if(  point.x < 1 || point.x > (int)getImage().cameraInfo.resolutionWidth-2
    || point.y < 1 || point.y > (int)getImage().cameraInfo.resolutionHeight-2)
    return 0;

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means
  Vector2<double> gradient;

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
  return gradient.angle();
}//end getPointsAngle

double ScanLineEdgelDetectorDifferential::calculateMeanAngle(double angle1,double angle2) const
{
  //calculate unit vectors for both angles and add them
  double x=cos(2*angle1)+cos(2*angle2);
  double y=sin(2*angle1)+sin(2*angle2);

  //calculate sum vectors angle
  return(atan2(y, x)/2);
}//end calculateMeanAngle
