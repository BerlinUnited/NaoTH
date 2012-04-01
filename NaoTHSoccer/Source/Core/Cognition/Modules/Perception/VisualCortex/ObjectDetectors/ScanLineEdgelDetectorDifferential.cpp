/* 
 * File:   ScanLineEdgelDetectorDifferential.cpp
 * Author: claas
 * Author: Heinrich Mellmann
 * 
 * Created on 14. März 2011, 14:22
 */

#include "ScanLineEdgelDetectorDifferential.h"

#include "Tools/CameraGeometry.h"
#include "Tools/Debug/NaoTHAssert.h"

ScanLineEdgelDetectorDifferential::ScanLineEdgelDetectorDifferential()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetectorDifferential:scanlines", "mark the scan lines", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_end_points_on_field", "...", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_estimated_obstacles", "...", false);
}


ScanLineEdgelDetectorDifferential::~ScanLineEdgelDetectorDifferential()
{}


void ScanLineEdgelDetectorDifferential::execute()
{
  // reset the percept
  getScanLineEdgelPercept().reset();


  // scan only inside the estimated field region
  Vector2<unsigned int> beginField = getFieldPercept().getLargestValidRect(getCameraMatrix().horizon).points[0];

  // horizontal stepsize between the scanlines
  int step = (getImage().cameraInfo.resolutionWidth - 1) / theParameters.scanline_count;
  int remaining_pixels = (getImage().cameraInfo.resolutionWidth - 1) % theParameters.scanline_count;

  // don't scan the lower lines in the image
  int borderY = getImage().cameraInfo.resolutionHeight - theParameters.pixel_border_y - 1;
  
  // start and endpoints for the scanlines
  Vector2<int> start((step + remaining_pixels) / 2, borderY);
  Vector2<int> end((step + remaining_pixels) / 2, beginField.y );
  

  for (int i = 0; i < theParameters.scanline_count; i++)
  {
    // don't scan the own body
    start = getBodyContour().returnFirstFreeCell(start);
    ASSERT(start.x >= 0 && start.x <= 320 && start.y >= 0 && start.y <= 240);

    end.x = start.x;

    // execute the scan
    ScanLineEdgelPercept::EndPoint endPoint = scanForEdgels(i, start, end);
    
    
    // check if endpoint is not same as the begin of the scanline
    if(endPoint.posInImage.y < borderY)
    {
      // project it on the ground
      CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getImage().cameraInfo,
        endPoint.posInImage.x,
        endPoint.posInImage.y,
        0.0,
        endPoint.posOnField);

      getScanLineEdgelPercept().endPoints.push_back(endPoint);
    }//end if


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

  //obstacle detection vizualization
  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_estimated_obstacles",
    unsigned int size = getScanLineEdgelPercept().endPoints.size();
    if (size >=2)
    {
      int pointer = 0;
      for (unsigned int i = 0;  i < getScanLineEdgelPercept().endPoints.size(); i++)
      {
        const ScanLineEdgelPercept::EndPoint& point = getScanLineEdgelPercept().endPoints[i];
        if (point.color == (int) ColorClasses::white)
        {
          pointer++;
        } 
        else
        {
          if (pointer > 1)
          {
            while (pointer != 0)
            {
              const ScanLineEdgelPercept::EndPoint& point2 = getScanLineEdgelPercept().endPoints[i-pointer];
              RECT_PX(ColorClasses::green, point2.posInImage.x - 3, point2.posInImage.y - 3, point2.posInImage.x + 3, point2.posInImage.y + 3);
              pointer--;
            }
          }
          pointer = 0;
        }

      }

    }
    );

  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_end_points_on_field",
    FIELD_DRAWING_CONTEXT;

    for(unsigned int i = 0; i < getScanLineEdgelPercept().endPoints.size(); i++)
    {
      const ScanLineEdgelPercept::EndPoint& point = getScanLineEdgelPercept().endPoints[i];

      if(point.posInImage.y < 10) // close to the top
        PEN("009900", 20);
      else
        PEN(ColorClasses::colorClassToHex(point.color), 20);

      CIRCLE(point.posOnField.x, point.posOnField.y, 10);
      if(i > 0)
      {
        const ScanLineEdgelPercept::EndPoint& last_point = getScanLineEdgelPercept().endPoints[i-1];
        LINE(last_point.posOnField.x, last_point.posOnField.y, point.posOnField.x, point.posOnField.y);
      }
    }//end for
  );

}//end execute



// scanForEdgelsHTWK
ScanLineEdgelPercept::EndPoint ScanLineEdgelDetectorDifferential::scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end) const
{
  Vector2<int> point(start);
  //Vector2<int> last_point(start);
  //Vector2<int> last_min_point(start); // the last point corresponding to lastMinPixelBrightnessDiff
  Vector2<int> lastGreenPoint(start); // needed for the endpoint

  // line scanning state
  //bool lineBeginFound = false;
  //results
  Edgel edgel; 

  //Pixel pixel = getImage().get(point.x, point.y);

  int t_edge = theParameters.brightness_threshold * 2;
  int g_min = t_edge;
  int g_max = -t_edge;
  int x_peak = 0;
  int f_last = getImage().getY(point.x, point.y);


  bool begin_found = false;
  int begin_y = 0;

  int begin_of_segment = point.y;

  // just go up
  for(;point.y >= end.y; point.y--)
  {
    // get the pixel color
    //getImage().get(point.x, point.y, pixel);

    // TODO: possible optimization
    //ColorClasses::Color thisPixelColor = (getColorClassificationModel().getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c))?ColorClasses::green:ColorClasses::none;

    int f_x = getImage().getY(point.x, point.y);
    int g = f_x - f_last;
    
    
    if(g > g_max)
    {
      /*
      Pixel pixel = getImage().get(point.x, x_peak);
      bool green = getColorClassificationModel().getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
      */
      if(g_min < -t_edge)// end found
      {

        DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
          // estimate the color of the past segment
          ColorClasses::Color color = estimateColorOfSegment(
            Vector2<int>(point.x, begin_of_segment),
            Vector2<int>(point.x, x_peak)
            );

          LINE_PX(color, point.x, begin_of_segment, point.x, x_peak);

          if(color == ColorClasses::green)
          {
            lastGreenPoint.y = x_peak;
          }
        );


        begin_of_segment = x_peak;



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
      /*
      Pixel pixel = getImage().get(point.x, x_peak);
      bool green = getColorClassificationModel().getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
      */
      if(g_max > t_edge)// begin found
      {
        DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
          POINT_PX(ColorClasses::blue, start.x, (int)x_peak);
        );

       
        DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetectorDifferential:mark_edgels",
          // estimate the color of the past segment
          ColorClasses::Color color = estimateColorOfSegment(
            Vector2<int>(point.x, begin_of_segment),
            Vector2<int>(point.x, x_peak)
            );

          LINE_PX(color, point.x, begin_of_segment, point.x, x_peak);

          if(color == ColorClasses::green)
          {
            lastGreenPoint.y = x_peak;
          }
        );

        begin_of_segment = x_peak;
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
  if( estimateColorOfSegment(
    Vector2<int>(point.x, begin_of_segment),
    Vector2<int>(point.x, x_peak)
    ) == ColorClasses::green)
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
  const int numberOfSamples = min(length, 5);
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
