/* 
 * File:   ScanLineEdgelDetectorDifferential.cpp
 * Author: claas
 * Author: Heinrich Mellmann
 * 
 * Created on 14. march 2011, 14:22
 */

#include "ScanLineEdgelDetectorDifferential.h"

#include "Tools/CameraGeometry.h"
#include "Tools/Debug/DebugModify.h"

ScanLineEdgelDetectorDifferential::ScanLineEdgelDetectorDifferential()
:
  cameraID(CameraInfo::Top),
  current_scanlineID(-1)
{
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_double_edgels", "mark the edgels on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_endpoints", "mark the endpints on the image", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetectorDifferential:scanlines", "mark the scan lines", false);
  DEBUG_REQUEST_REGISTER("Vision:Detectors:ScanLineEdgelDetectorDifferential:expected_line_width", "", false);
}

ScanLineEdgelDetectorDifferential::~ScanLineEdgelDetectorDifferential()
{}

void ScanLineEdgelDetectorDifferential::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);
  getScanLineEdgelPercept().reset();

  // needs a valid field polygon
  //if(!getFieldPerceptRaw().valid) {
  //  return;
  //}

  double h = 500.0;
  double d_2 = 50/2;
 
  int horizon_height = (int)(std::min(getArtificialHorizon().begin().y, getArtificialHorizon().end().y)+0.5);
  // clamp it to the image
  horizon_height = Math::clamp(horizon_height,0,(int)getImage().height());

  // scan only inside the estimated field region
  //horizon_height = getFieldPerceptRaw().getValidField().points[0].y;

  for(int i = 0; i < (int) naoth::IMAGE_HEIGHT; i++)
  {
    // reset
    vertical_confidence[i] = 0.0;

    // no clculation above horizon
    if (i < horizon_height) continue;

    double x = getImage().cameraInfo.getFocalLength();
    double z = -i + getImage().cameraInfo.getOpticalCenterY();
    double alpha = atan2(z, x);
    double w = Math::normalize(-alpha + getCameraMatrix().rotation.getYAngle());
    
    double dist = h*tan(Math::pi_2 - w);
    double g = atan2(dist + d_2, h) - atan2(dist - d_2, h);
    double rad_per_px = getImage().cameraInfo.getOpeningAngleWidth()/getImage().width();
    double v = g / rad_per_px;
    
    vertical_confidence[i] = std::max(0.0,v);
  }//end for

  DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:expected_line_width",
    for(int i = 0; i < (int) naoth::IMAGE_HEIGHT; i++)
    {
      unsigned char c = (unsigned char)(vertical_confidence[i]);
      POINT_PX(c, 0, 0, c, i);
    }
  );

  
  // horizontal stepsize between the scanlines
  int step = (getImage().width() - 1) / (theParameters.scanline_count - 1);
  //int remaining_pixels = (getImage().width() - 1) % theParameters.scanline_count;

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

    current_scanlineID = i;

    // vertical scanlines
    end.x = start.x;

    // execute the scan
    ScanLineEdgelPercept::EndPoint endPoint = scanForEdgels(i, start, end);
    
    // check if endpoint is not same as the begin of the scanline
    if(endPoint.posInImage.y >= borderY) {
      endPoint.color = ColorClasses::none;
    }

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

  DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_edgels",
    for(unsigned int i = 0; i < getScanLineEdgelPercept().edgels.size(); i++) {
      const Edgel& edgel = getScanLineEdgelPercept().edgels[i];
      Vector2d g1(10.0,0);
      g1.rotate(edgel.angle);
      g1.rotateRight();
      LINE_PX(ColorClasses::black,edgel.point.x, edgel.point.y, edgel.point.x + (int)(g1.x), edgel.point.y + (int)(g1.y));
    }
  );

  // mark finished valid edgels
  DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_double_edgels",
    for(unsigned int i = 0; i < getScanLineEdgelPercept().numOfSeenEdgels; i++)
    {
      const DoubleEdgel& double_edgel = getScanLineEdgelPercept().scanLineEdgels[i];
      LINE_PX(ColorClasses::red   ,double_edgel.begin.x ,double_edgel.begin.y ,double_edgel.begin.x + (int)(10 * cos(double_edgel.begin_angle))   ,double_edgel.begin.y + (int)(10 * sin(double_edgel.begin_angle)));
      LINE_PX(ColorClasses::blue  ,double_edgel.center.x,double_edgel.center.y,double_edgel.center.x + (int)(10 * cos(double_edgel.center_angle)) ,double_edgel.center.y + (int)(10 * sin(double_edgel.center_angle)));
      LINE_PX(ColorClasses::black ,double_edgel.end.x   ,double_edgel.end.y   ,double_edgel.end.x + (int)(10 * cos(double_edgel.end_angle))       ,double_edgel.end.y + (int)(10 * sin(double_edgel.end_angle)));
    }
  );
  
  DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_endpoints",
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
ScanLineEdgelPercept::EndPoint ScanLineEdgelDetectorDifferential::scanForEdgels(int scan_id, const Vector2i& start, const Vector2i& end)
{
  Vector2i point(start);
  point.y -= 2; // make one step

  Vector2i lastGreenPoint(start); // needed for the endpoint

  //results
  DoubleEdgel double_edgel;
  Edgel edgel;

  //Pixel pixel = getImage().get(point.x, point.y);

  Vector2i peak_point(start);

  // initialize
  int t_edge = theParameters.brightness_threshold * 2;
  int g_min = t_edge;
  int g_max = -t_edge;
  int& x_peak = peak_point.y;//0;
  int f_last = getImage().getY(point.x, point.y);

  double line_thicknes_param = 1.0;
  MODIFY("ScanLineEdgelPercept:line_thicknes_param", line_thicknes_param);
  
  //
  double greenCount = 0;
  double segmentLength = 0;

  bool begin_found = false;
  int begin_y = 0;

//  int begin_of_segment = point.y;

  // just go up
  for(;point.y >= end.y; point.y -= 2)
  {
    // get the pixel color
    Pixel pixel = getImage().get(point.x, x_peak);
    bool green = getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
    if(green) greenCount++;
    segmentLength++;

    // get the brightness chanel
    int f_x = getImage().getY(point.x, point.y);
    int g = f_x - f_last;
    
    
    if(g > g_max)
    {
      /*
      DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_edgels",
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
        greenCount = 0;
        segmentLength = 1;

        // new end edgel
        add_edgel(start.x, x_peak);

        if(begin_found) // secure edgel
        {
          DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_double_edgels",
            POINT_PX(ColorClasses::red, start.x, x_peak);
          );
          begin_found = false;


          //a new double edgel
          double_edgel.begin.x = start.x;
          double_edgel.begin.y = begin_y;
          double_edgel.begin_angle = getPointsAngle(double_edgel.begin);

          double_edgel.end.x = start.x;
          double_edgel.end.y = x_peak;
          double_edgel.end_angle = Math::normalizeAngle(getPointsAngle(double_edgel.end) + Math::pi);

          double_edgel.center = (double_edgel.end + double_edgel.begin) / 2;
          double_edgel.center_angle = calculateMeanAngle(double_edgel.begin_angle, double_edgel.end_angle);
          double_edgel.valid = fabs(Math::normalizeAngle(double_edgel.begin_angle - double_edgel.end_angle)) <= theParameters.double_edgel_angle_threshold;

          double_edgel.ScanLineID = current_scanlineID;

          double edgel_c = vertical_confidence[double_edgel.center.y];
          if(double_edgel.valid && 
            (double_edgel.end - double_edgel.begin).abs2() > edgel_c*edgel_c*line_thicknes_param*line_thicknes_param //&&
            //(edgel.end - edgel.begin).abs2() < 4*edgel_c*edgel_c*line_thicknes_param*line_thicknes_param
            )
          {
            double_edgel.ScanLineID = scan_id;
            getScanLineEdgelPercept().add(double_edgel);
          }

        }
        else
        {
          DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_double_edgels",
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
        DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:mark_double_edgels",
          POINT_PX(ColorClasses::blue, start.x, (int)x_peak);
        );

        // new begin edgel
        add_edgel(start.x, x_peak);

        //if( greenCount/segmentLength > 0.3 ) {
        if(estimateColorOfSegment(lastGreenPoint, peak_point) == ColorClasses::green) {
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


    DEBUG_REQUEST("Vision:Detectors:ScanLineEdgelDetectorDifferential:scanlines",
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


ColorClasses::Color ScanLineEdgelDetectorDifferential::estimateColorOfSegment(const Vector2i& begin, const Vector2i& end) const
{
  ASSERT(begin.x == end.x && end.y <= begin.y);

  int length = begin.y - end.y;
  int numberOfGreen = 0;
  const int numberOfSamples = std::min(length, 20);
  Vector2i point(begin);
  Pixel pixel;

  int meanY = 0;

  for(int i = 0; i < numberOfSamples; i++)
  {
    int k = Math::random(length+1); // number in [0,length]
    point.y = end.y + k;
    getImage().get(point.x, point.y, pixel);
    numberOfGreen += getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c);
    meanY += pixel.y;
  }//end for

  if(length > 0) meanY /= length;

  return (numberOfGreen > numberOfSamples/2) ? ColorClasses::green : ColorClasses::none;
}//end estimateColorOfSegment


double ScanLineEdgelDetectorDifferential::getPointsAngle(const Vector2i& point) const
{
  // no angle at the border (shouldn't happen)
  if(  point.x < 1 || point.x > (int)getImage().width()-2
    || point.y < 1 || point.y > (int)getImage().height()-2)
    return 0;

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means
  Vector2d gradient;

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
  /*
  Vector2d a (1,0);
  a.rotate(angle1);
  Vector2d b (1,0);
  b.rotate(angle2);

  return (a+b).angle();
  */

  //calculate unit vectors for both angles and add them
  double x=cos(2*angle1)+cos(2*angle2);
  double y=sin(2*angle1)+sin(2*angle2);

  //calculate sum vectors angle
  return(atan2(y, x)/2);
}//end calculateMeanAngle
