/* 
 * File:   ScanLineEdgelDetector.cpp
 * Author: claas
 * 
 * Created on 14. march 2011, 14:22
 */

#include "ScanLineEdgelDetector.h"

#include "Tools/CameraGeometry.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

ScanLineEdgelDetector::ScanLineEdgelDetector()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:mark_edgels", "mark the edgels on the image", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:scanlines", "mark the scan lines", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:mark_end_points_on_field", "...", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:use_iterative_edgel_detection", "...", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:mark_estimated_obstacles", "...", false);
}


ScanLineEdgelDetector::~ScanLineEdgelDetector()
{}


void ScanLineEdgelDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);
  getScanLineEdgelPercept().reset();

  cameraBrighness = getCurrentCameraSettings().data[CameraSettings::Brightness];
  if(cameraBrighness <= 0 || cameraBrighness > 255)
  {
    edgelBrightnessLevel = BRIGHTNESS;
    edgelGrayThresholdLevel = GRAY_THRESHOLD;
  }
  else
  {
    edgelBrightnessLevel = (unsigned int) (cameraBrighness / 2);
    edgelGrayThresholdLevel =(unsigned int) (cameraBrighness / 10 );
  }
  edgelBrightnessLevel = BRIGHTNESS;
  edgelGrayThresholdLevel = GRAY_THRESHOLD;

  STOPWATCH_START("ScanLineEdgelDetector ~ find edgels");
  bool use_iterative_edgel_detection = false;
  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:use_iterative_edgel_detection", 
    use_iterative_edgel_detection = true;
  );

  if(use_iterative_edgel_detection)
    iterative_edgel_detection();
  else
    integrated_edgel_detection();
  STOPWATCH_STOP("ScanLineEdgelDetector ~ find edgels");

}//end execute


void ScanLineEdgelDetector::integrated_edgel_detection()
{
  // TODO: fix it
  Vector2<unsigned int> beginField = getFieldPercept().getLargestValidRect(getArtificialHorizon()).points[0];
  int scanLineID = 0;

  int step = (getImage().cameraInfo.resolutionWidth - 1) / (SCANLINE_COUNT);


  int borderY = getImage().cameraInfo.resolutionHeight - PIXEL_BORDER - 1;
  Vector2<int> start(step / 2, borderY);
  Vector2<int> end(step / 2, max((int) beginField.y - 10, 0) );
  

  for (;start.x < (int) getImage().cameraInfo.resolutionWidth;)
  {
    start = getBodyContour().returnFirstFreeCell(start);
    ASSERT(getImage().isInside(start.x, start.y));
    end.x = start.x;
    ScanLineEdgelPercept::EndPoint endPoint = scanForEdgels(scanLineID, start, end);
    //check if endpoint is not same as the begin of the scanline
    //if(endPoint.posInImage.y < borderY)
    {
      endPoint.valid = CameraGeometry::imagePixelToFieldCoord(
        getCameraMatrix(),
        getImage().cameraInfo,
        endPoint.posInImage.x,
        endPoint.posInImage.y,
        0.0,
        endPoint.posOnField);

      // HACK: don't take endpoints on the body contur
      if(endPoint.posInImage.y < start.y - 1)
        getScanLineEdgelPercept().endPoints.push_back(endPoint);
    }
    scanLineID++;
    start.y = getImage().cameraInfo.resolutionHeight - PIXEL_BORDER - 1;
    start.x += step;
  }//end for


  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
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
  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_estimated_obstacles",
		unsigned int size = static_cast<unsigned int> (getScanLineEdgelPercept().endPoints.size());
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

  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_end_points_on_field",
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

}//end integrated_edgel_detection


void ScanLineEdgelDetector::iterative_edgel_detection()
{
  /*
  // TODO: fixit
  Vector2<unsigned int> beginField = getFieldPercept().getLargestValidRect(getCameraMatrix().horizon).points[0];
  int resumedScanCount;
  int scanLineID = 0;

  int step = (getImage().cameraInfo.resolutionWidth - 1) / (SCANLINE_COUNT);
  Vector2<int> start(step / 2, getImage().cameraInfo.resolutionHeight - PIXEL_BORDER - 1);
  Vector2<int> end(step / 2, max((int) beginField.y - 10, 0) );

  for (; start.x < (int) getImage().cameraInfo.resolutionWidth; start.x += step)
  {
    end.x = start.x;
    DoubleEdgel tmp = getEdgel(start, end);
    tmp.runID = 0;
    tmp.ScanLineID = scanLineID;


    if(tmp.center.x != 0 && tmp.center.y != 0)
    {
      getScanLineEdgelPercept().add(tmp);
    }
//    if (tmp.valid && tmp.center.x != 0 && tmp.end.x != 0)
    {
      DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
        LINE_PX(ColorClasses::pink,tmp.begin.x,tmp.begin.y,tmp.begin.x + (int)(10 * cos(tmp.begin_angle)),tmp.begin.y + (int)(10 * sin(tmp.begin_angle)));
        LINE_PX(ColorClasses::pink,tmp.end.x,tmp.end.y,tmp.end.x + (int)(10 * cos(tmp.end_angle + Math::pi)) ,tmp.end.y + (int)(10 * sin(tmp.end_angle + Math::pi)));
        LINE_PX(ColorClasses::black,tmp.center.x,tmp.center.y,tmp.center.x + (int)(10 * cos(tmp.center_angle)) ,tmp.center.y + (int)(10 * sin(tmp.center_angle)));
      );
    }//end if
    resumedScanCount = 0;
    while(tmp.end.y > end.y && SCANLINE_RESUME_COUNT > resumedScanCount++)
    {
      tmp = getEdgel(tmp.end, end);
      tmp.runID = resumedScanCount;// + 1;
      tmp.ScanLineID = scanLineID;

      if(tmp.center.x != 0 && tmp.center.y != 0)
      {
        getScanLineEdgelPercept().add(tmp);
      }
//      if (tmp.valid && tmp.center.x != 0 && tmp.end.x != 0)
      {
        DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
          LINE_PX(ColorClasses::black,tmp.begin.x,tmp.begin.y,tmp.begin.x + (int)(10 * cos(tmp.begin_angle)),tmp.begin.y + (int)(10 * sin(tmp.begin_angle)));
          LINE_PX(ColorClasses::black,tmp.end.x,tmp.end.y,tmp.end.x + (int)(10 * cos(tmp.end_angle + Math::pi)) ,tmp.end.y + (int)(10 * sin(tmp.end_angle + Math::pi)));
          LINE_PX(ColorClasses::pink,tmp.center.x,tmp.center.y,tmp.center.x + (int)(10 * cos(tmp.center_angle)) ,tmp.center.y + (int)(10 * sin(tmp.center_angle)));
        );
      }
    }//end while
    scanLineID++;
  }//end for
  */
}//end execute



ScanLineEdgelPercept::EndPoint ScanLineEdgelDetector::scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end)
{
  // count the off-green colors (needed for the endpoint)
  int whiteCount = 0;
  int skyblueCount = 0;
  int yellowCount = 0;

  // statistics of the green color
  double noGreenSeen=0; // pixel count where continously no green was seen
  double greenCount = 0;
  double totalGreenCount = 0;
  double greenValue = 0;
  double calculatedGreenValue = 0;

  double segmentLengthNoWhite = 0;
  
  RingBufferWithSum<double, 30> movingWindow;
  
  // trac the derivaive of the brightness
  int lastPixelBrightness = 0;
  int lastPixelBrightnessDiff = 0;
  int lastMinPixelBrightnessDiff = 0;
 

  BresenhamLineScan scanLine(start, end);
  Vector2<int> point(start);
  Vector2<int> last_point(start);
  Vector2<int> last_min_point(start); // the last point corresponding to lastMinPixelBrightnessDiff
  Vector2<int> lastGreenPoint(start); // needed for the endpoint

  // line scanning state
  bool lineBeginFound = false;
  //results
  DoubleEdgel edgel; 

  Pixel pixel = getImage().get(point.x,point.y);
  int thisPixelBrightness = pixel.y;
  ColorClasses::Color thisPixelColor = getColorClassificationModel().getColorClass(pixel);
  

  // don't scan the two bottom lines
  for(int i = 3; i < scanLine.numberOfPixels; i +=2 )
  {
    getImage().get(point.x,point.y,pixel);
    thisPixelBrightness = pixel.y;
    
    //thisPixelColor = getColorClassificationModel().getColorClass(pixel); // 12ms

    // TODO: possible optimization
    thisPixelColor = (getColorClassificationModel().getFieldColorPercept().isFieldColor(pixel.a, pixel.b, pixel.c))?ColorClasses::green:ColorClasses::none;

    DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:scanlines",
      //int b_offset = thisPixelBrightness / 10;
      int b_offset = 0;
      POINT_PX(thisPixelColor, point.x + b_offset, point.y);
    );

    if (lineBeginFound) // scan a line
    {
      // end of the line found
      if( thisPixelBrightness - calculatedGreenValue <= (int)edgelGrayThresholdLevel ||
          thisPixelColor == ColorClasses::green)
      {
        lineBeginFound = false;
        edgel.end = last_min_point;
        edgel.end_angle = Math::normalizeAngle(getPointsAngle(edgel.end) + Math::pi);

        DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
          POINT_PX(ColorClasses::yellow, last_min_point.x, last_min_point.y);
        );
        edgel.center = (edgel.end + edgel.begin) / 2;
        edgel.center_angle = calculateMeanAngle(edgel.begin_angle, edgel.end_angle);

        // check the angle
        edgel.valid = fabs(Math::normalizeAngle(edgel.begin_angle - edgel.end_angle)) <= EDGEL_ANGLE_THRESHOLD;
      }
    }
    else // scan the ground
    {
      // is this the right place for it?
      segmentLengthNoWhite++;

      if( greenCount > 5 &&
          thisPixelBrightness - calculatedGreenValue > (int)edgelGrayThresholdLevel &&
          thisPixelColor != ColorClasses::green )
      {
        lineBeginFound = true;
        lastMinPixelBrightnessDiff = 0;

        // last engel
        edgel.valid = edgel.valid && noGreenSeen <= 20;
        if(edgel.valid)
        {
          edgel.ScanLineID = scan_id;
          getScanLineEdgelPercept().add(edgel);

          // mark finished valid edgels
          DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
            LINE_PX(ColorClasses::black,edgel.begin.x, edgel.begin.y,    edgel.begin.x  + (int)(10 * cos(edgel.begin_angle)),edgel.begin.y + (int)(10 * sin(edgel.begin_angle)));
            LINE_PX(ColorClasses::black,edgel.end.x,   edgel.end.y,      edgel.end.x    + (int)(10 * cos(edgel.end_angle + Math::pi)) ,edgel.end.y + (int)(10 * sin(edgel.end_angle + Math::pi)));
            LINE_PX(ColorClasses::pink ,edgel.center.x,edgel.center.y,edgel.center.x + (int)(10 * cos(edgel.center_angle)) ,edgel.center.y + (int)(10 * sin(edgel.center_angle)));
          );
        }

        DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
          POINT_PX(ColorClasses::red, last_point.x, last_point.y);
        );
        // begin a new edgel
        edgel.begin = last_point;
        edgel.begin_angle = getPointsAngle(edgel.begin);
      }
    }//end else

    

    if(thisPixelColor == ColorClasses::green || thisPixelColor == ColorClasses::orange) // ignore the ball
    {
      double greenDensity = movingWindow.getSum()/movingWindow.size();
      if(thisPixelColor == ColorClasses::green && greenDensity > 0.3)
      {
        greenValue += thisPixelBrightness;
        lastGreenPoint = point;
      }
      greenCount++;
      totalGreenCount++;
      noGreenSeen = 0;
      whiteCount = 0;
      skyblueCount = 0;
      yellowCount = 0;
      movingWindow.add(1.0);
    }
    else
    {
      // count the colors
      whiteCount += thisPixelColor == ColorClasses::white;
      skyblueCount += thisPixelColor == ColorClasses::skyblue;
      yellowCount += thisPixelColor == ColorClasses::yellow;

      greenCount = 0;
      greenValue = 0;
      noGreenSeen++;
      movingWindow.add(0.0);
    }


    // calculate the mean green value
    if (greenCount > 5)
    {
      calculatedGreenValue = greenValue / greenCount;
    }

    last_point = point;
    lastPixelBrightnessDiff = thisPixelBrightness - lastPixelBrightness;
    lastPixelBrightness = thisPixelBrightness;

    if(lastPixelBrightnessDiff < lastMinPixelBrightnessDiff)
    {
      lastMinPixelBrightnessDiff = lastPixelBrightnessDiff;
      last_min_point = point;
    }

    // this one is a kind of slow but does not crash..
    scanLine.getNext(point);
    scanLine.getNext(point);
    //point.y--; // CRASHES just go up
  }//end for

  // for the last edgel
  //after a detected line should come green pixels &
  //both sobel angles should be somehow parallel
  edgel.valid = edgel.valid && noGreenSeen <= 20;

  if(edgel.valid)
  {
    edgel.ScanLineID = scan_id;
    getScanLineEdgelPercept().add(edgel);

    DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
      LINE_PX(ColorClasses::black,edgel.begin.x, edgel.begin.y,    edgel.begin.x  + (int)(10 * cos(edgel.begin_angle)),edgel.begin.y + (int)(10 * sin(edgel.begin_angle)));
      LINE_PX(ColorClasses::black,edgel.end.x,   edgel.end.y,      edgel.end.x    + (int)(10 * cos(edgel.end_angle + Math::pi)) ,edgel.end.y + (int)(10 * sin(edgel.end_angle + Math::pi)));
      LINE_PX(ColorClasses::pink ,edgel.center.x,edgel.center.y,edgel.center.x + (int)(10 * cos(edgel.center_angle)) ,edgel.center.y + (int)(10 * sin(edgel.center_angle)));
    );
   
  }


  // determine the maximum off-green color
  ColorClasses::Color maxLastColor = ColorClasses::green;
  if(skyblueCount > whiteCount && skyblueCount > yellowCount)
  {
    if(skyblueCount > 5) maxLastColor = ColorClasses::skyblue;
  }
  else
  {
    if(yellowCount >= whiteCount)
    {  
      if(yellowCount > 5) maxLastColor = ColorClasses::yellow;
    }
    else
    {
      if(whiteCount > 5) maxLastColor = ColorClasses::white;
    }
  }//end else

  if(maxLastColor == ColorClasses::green && noGreenSeen > 7)
  {
    maxLastColor = ColorClasses::none;
  }

  ScanLineEdgelPercept::EndPoint endPoint;
  endPoint.posInImage = lastGreenPoint;
  endPoint.color = maxLastColor;
  endPoint.ScanLineID = scan_id;

  return endPoint;
}//end scanForEdgels

//////////////////////////////////////////////////////////
// scans at given x-coordinate to the top & cancels at  //
// field end. starts at bottom line or given y      //
//////////////////////////////////////////////////////////
DoubleEdgel ScanLineEdgelDetector::getEdgel(const Vector2<int>& start, const Vector2<int>& end)
{
  DoubleEdgel edgel; //result
  unsigned int greenValue = 0;
  unsigned int greenCount = 0;
  unsigned int totalGreenCount = 0;
  unsigned int calculatedGreenValue = 0;

  unsigned int noGreenSeen=0; // pixel count where continously no green was seen

  bool lineBeginFound = false;
  bool lineEndFound = false;
  unsigned int thisPixelBrightness;
  ColorClasses::Color thisPixelColor = ColorClasses::none;

  BresenhamLineScan scanLine(start, end);
  Vector2<int> point(start);
  for(int i = 1; i < scanLine.numberOfPixels; i++ )
  {
    //scanline outside field / obstacle / check
    if (noGreenSeen > MAX_POSSIBLE_LINE_THICKNESS)
    {
      // mark the border
      DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:scanlines",
        CIRCLE_PX(ColorClasses::red, point.x, point.y, 5);
      );
      break;
    }

    Pixel pixel = getImage().get(point.x,point.y);
    thisPixelBrightness = pixel.y;
    thisPixelColor = getColorClassificationModel().getColorClass(pixel);

    DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:scanlines",
      POINT_PX(thisPixelColor, point.x, point.y);
    );

    if (greenCount > 5)
    {
      calculatedGreenValue = (int)((greenValue / greenCount) + 0.5);
    }
    if (!lineBeginFound)
    {
      //searching for line beginning
      if
      ( greenCount > 5 &&
        (unsigned int) abs((int)(thisPixelBrightness -  calculatedGreenValue)) > edgelGrayThresholdLevel &&
         thisPixelColor != ColorClasses::green
      )
      {
        lineBeginFound = true;
        edgel.begin = point;
        edgel.begin_angle = getPointsAngle(edgel.begin);
        noGreenSeen = 1;
      }
      else
      {
        if (thisPixelColor == ColorClasses::green)
        {
          noGreenSeen = 0;
          greenCount++;
          totalGreenCount++;
          greenValue += thisPixelBrightness;
        }
        else
        {
          noGreenSeen++;
        }
      }
    }
    else
    {
      //searching for line end
      if (!lineEndFound && (thisPixelBrightness - (int)calculatedGreenValue <= edgelGrayThresholdLevel || thisPixelColor == ColorClasses::green))
      {
        noGreenSeen = 0;
        lineEndFound = true;
        edgel.end.x = point.x;
        edgel.end.y = point.y;// + STEP_SIZE; // use last point;
        edgel.end_angle = Math::normalizeAngle(getPointsAngle(edgel.end) + Math::pi);

        edgel.center = (edgel.end + edgel.begin) / 2;
        edgel.center_angle = calculateMeanAngle(edgel.begin_angle, edgel.end_angle);
        //TODO: check if step back necessary
        //edgel.end.y += STEP_SIZE; // use last point;
      }
      else if (lineEndFound && thisPixelColor == ColorClasses::green)
      {
        //after a detected line should come green pixels &
        //both sobel angles should be somehow parallel
        if
        (
          fabs(Math::normalizeAngle(edgel.begin_angle - edgel.end_angle)) > EDGEL_ANGLE_THRESHOLD
          || noGreenSeen > 20
        )
        {
          edgel.valid = false;
        }
        else
        {
          edgel.valid = true;
          break;
        }
      }
      noGreenSeen++;
    }//end else
    scanLine.getNext(point);

  }//end for

  return edgel;
}//end getEdgel


///////////GET ANGLE FROM POINT///////////////////////////
double ScanLineEdgelDetector::getPointsAngle(Vector2<int>& point)
{
  int pixelEnvironment[3][3];
  int x, y;

  //just get pixelenvironment of the pixel in question
  //for pixels outside the image the last pixel in there row/column is taken respectively
  for(int offsetX=-1; offsetX < 2; offsetX++)
  {
    for(int offsetY=-1; offsetY < 2; offsetY++)
    {
      x = point.x + offsetX;
      y = point.y + offsetY;
      x = Math::clamp(x, 1, (signed int)getImage().cameraInfo.resolutionWidth-1);
      y = Math::clamp(y, 1, (signed int)getImage().cameraInfo.resolutionHeight-1);
      Pixel pixel = getImage().get(x,y);
      pixelEnvironment[offsetX+1][offsetY+1] = pixel.y;
    }//end for offsetY
  }//end for

  //apply Sobel Operator on (pointX, pointY)
  //and calculate gradient in x and y direction by that means
  double gradientX =
           pixelEnvironment[0][2]
        +2*pixelEnvironment[1][2]
        +  pixelEnvironment[2][2]
        -  pixelEnvironment[0][0]
        -2*pixelEnvironment[1][0]
        -  pixelEnvironment[2][0];

  double gradientY =
           pixelEnvironment[0][0]
        +2*pixelEnvironment[0][1]
        +  pixelEnvironment[0][2]
        -  pixelEnvironment[2][0]
        -2*pixelEnvironment[2][1]
        -  pixelEnvironment[2][2];

  //calculate the corresponding angle from the gradients
  return atan2(gradientY, gradientX);
}//end getPointsAngle

double ScanLineEdgelDetector::calculateMeanAngle(double angle1,double angle2)
{
  //calculate unit vectors for both angles and add them
  double x=cos(2*angle1)+cos(2*angle2);
  double y=sin(2*angle1)+sin(2*angle2);

  //calculate sum vectors angle
  return(atan2(y, x)/2);
}//end calculateMeanAngle
