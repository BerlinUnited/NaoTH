/* 
 * File:   ScanLineEdgelDetector.cpp
 * Author: claas
 * 
 * Created on 14. März 2011, 14:22
 */

#include "ScanLineEdgelDetector.h"

#include "Tools/CameraGeometry.h"

ScanLineEdgelDetector::ScanLineEdgelDetector()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:use_iterative_edgel_detection", "...", true);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:mark_edgels", "mark the edgels on the image", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:scanlines", "mark the scan lines", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:scanlines_noisy", "mark the scan lines", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:mark_end_points", "...", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:mark_end_points_on_field", "...", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ScanLineEdgelDetector:mark_estimated_obstacles", "...", false);
}//end constructor

ScanLineEdgelDetector::~ScanLineEdgelDetector()
{
}//end destructor

void ScanLineEdgelDetector::execute()
{
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
  Vector2<unsigned int> beginField = getFieldPercept().getLargestValidRect(getCameraMatrix().horizon).points[0];
  int scanLineID = 0;

  int step = (getImage().cameraInfo.resolutionWidth - 1) / (SCANLINE_COUNT);
  Vector2<int> start(step / 2, getImage().cameraInfo.resolutionHeight - PIXEL_BORDER - 1);
  Vector2<int> end(step / 2, max((int) beginField.y - 10, 0) );

  while (start.x < (int) getImage().cameraInfo.resolutionWidth)
  {
    start = getBodyContour().returnFirstFreeCell(start);
    end.x = start.x;
    ScanLineEdgelPercept::EndPoint endPoint = scanForEdgels(scanLineID, start, end);

    CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), 
      getImage().cameraInfo,
      endPoint.posInImage.x, 
      endPoint.posInImage.y, 
      0.0,
      endPoint.posOnField);

    getScanLineEdgelPercept().endPoints.push_back(endPoint);
    scanLineID++;
    start.x += step;
  }//end while

  drawDebug();

}//end integrated_edgel_detection

void ScanLineEdgelDetector::iterative_edgel_detection()
{
  Vector2<int> beginField = getFieldPercept().getLargestValidRect(getCameraMatrix().horizon).points[0];
  int resumedScanCount;
  unsigned int scanLineID = 0;
  Edgel tmp;

  int stepX = (getImage().cameraInfo.resolutionWidth - 1) / (SCANLINE_COUNT);
  Vector2<int> start(stepX / 2, getImage().cameraInfo.resolutionHeight - PIXEL_BORDER - 1);
  Vector2<int> end(stepX / 2, max((int) beginField.y - 10, 0) );

  for (; start.x < (int) getImage().cameraInfo.resolutionWidth; start.x += stepX)
  {
    scanlineStates[scanLineID].reset(start, scanLineID);

    start = getBodyContour().returnFirstFreeCell(start);
    end.x = start.x;
    tmp.end = start;
    resumedScanCount = 0;
    while(tmp.end.y > (int) end.y && SCANLINE_RESUME_COUNT >= resumedScanCount++)
    {
      tmp = getEdgel(tmp.end, end, scanlineStates[scanLineID]);
      tmp.runID = resumedScanCount;
      tmp.ScanLineID = scanLineID;
      if(tmp.center.x != 0 && tmp.center.y != 0)
      {
        getScanLineEdgelPercept().add(tmp);
        scanlineStates[scanLineID].lineEdgelCount++;
      }
      DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
        if (tmp.valid && tmp.center.x != 0 && tmp.end.x != 0)
        {
          LINE_PX(ColorClasses::black,tmp.begin.x,tmp.begin.y,tmp.begin.x + (int)(10 * cos(tmp.begin_angle)),tmp.begin.y + (int)(10 * sin(tmp.begin_angle)));
          LINE_PX(ColorClasses::black,tmp.end.x,tmp.end.y,tmp.end.x + (int)(10 * cos(tmp.end_angle)) ,tmp.end.y + (int)(10 * sin(tmp.end_angle)));
          LINE_PX(ColorClasses::pink,tmp.center.x,tmp.center.y,tmp.center.x + (int)(10 * cos(tmp.center_angle)) ,tmp.center.y + (int)(10 * sin(tmp.center_angle)));
        }
        else
        {
          LINE_PX(ColorClasses::red,tmp.begin.x,tmp.begin.y,tmp.begin.x + (int)(10 * cos(tmp.begin_angle)),tmp.begin.y + (int)(10 * sin(tmp.begin_angle)));
          LINE_PX(ColorClasses::yellow,tmp.end.x,tmp.end.y,tmp.end.x + (int)(10 * cos(tmp.end_angle)) ,tmp.end.y + (int)(10 * sin(tmp.end_angle)));
        }
      );
    }//end while

    if(scanlineStates[scanLineID].greenCount > 0)
    {
      scanlineStates[scanLineID].greenFound = true;
    }
    if(scanlineStates[scanLineID].noGreenSeen > 0)
    {
      scanlineStates[scanLineID].noGreenFound = true;
    }
    getScanlineEndPoint(scanlineStates[scanLineID]);
    getScanLineEdgelPercept().endPoints.push_back(scanlineStates[scanLineID].endPoint);

    scanLineID++;
  }//end for

  int maxY = getImage().cameraInfo.resolutionHeight - PIXEL_BORDER - 1;
  int minY = max((int) beginField.y - 10, 0);

  //use steps of size = 3 times scanline step size
  int stepY = 4 * (getImage().cameraInfo.resolutionHeight - 1) / (SCANLINE_COUNT);

  if(getFieldPercept().isValid())
  {
    ScanLineState dummy;
    bool skipNext = false;
    int lastFoundEndX = 0;
    for(scanLineID = 1; scanLineID < SCANLINE_COUNT - 1 ;scanLineID++)
    {
      if(scanLineID > 1)
      {
        start.x = stepX * (scanLineID - 2);
      }
      else
      {
        start.x = stepX * (scanLineID - 1);
      }

      if(lastFoundEndX < start.x)
      {
        lastFoundEndX = 0;
        if
        (
          (
            !scanlineStates[scanLineID].greenFound
            ||
            (
              scanlineStates[scanLineID].greenFound
              &&
              scanlineStates[scanLineID].lineEdgelCount == 0
            )
          )
          &&
          scanlineStates[scanLineID].noGreenFound
          &&
          getScanLineEdgelPercept().numOfSeenEdgels < MAX_NUMBER_OF_SCANLINE_EDGELS
        )
        {
          if(!skipNext || lastFoundEndX < start.x)
          {
            skipNext = false;
            start.y = maxY;
            end.x = start.x + MAX_POSSIBLE_LINE_THICKNESS * 2;
            end.y = maxY;
            for(; start.y > minY; start.y -= stepY)
            {
              end.y = start.y;
              tmp = getEdgel(start, end, dummy);
              tmp.runID = 0;
              tmp.ScanLineID = scanLineID + SCANLINE_COUNT;
              if(tmp.center.x != 0 && tmp.center.y != 0)
              {
                tmp.vertical = true;
                getScanLineEdgelPercept().add(tmp);
                skipNext = true;
                lastFoundEndX = max<int>(tmp.end.x, lastFoundEndX);
              }

              DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:scanlines",
                LINE_PX(ColorClasses::black, start.x, start.y, start.x + 5, start.y);
              );
              DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_edgels",
                LINE_PX(ColorClasses::black,tmp.begin.x,tmp.begin.y,tmp.begin.x + (int)(10 * cos(tmp.begin_angle)),tmp.begin.y + (int)(10 * sin(tmp.begin_angle)));
                LINE_PX(ColorClasses::black,tmp.end.x,tmp.end.y,tmp.end.x + (int)(10 * cos(tmp.end_angle + Math::pi)) ,tmp.end.y + (int)(10 * sin(tmp.end_angle + Math::pi)));
                LINE_PX(ColorClasses::pink,tmp.center.x,tmp.center.y,tmp.center.x + (int)(10 * cos(tmp.center_angle)) ,tmp.center.y + (int)(10 * sin(tmp.center_angle)));
              );
            }
            if(!skipNext)
            {
              lastFoundEndX = end.x;
            }
          }
        }
      }
    }
  }
  drawDebug();
}//end iterative_edgel_detection

ScanLineEdgelPercept::EndPoint ScanLineEdgelDetector::scanForEdgels(int scan_id, const Vector2<int>& start, const Vector2<int>& end)
{
  // count the off-green colors (needed for the endpoint)
  unsigned int whiteCount = 0;
  unsigned int skyblueCount = 0;
  unsigned int yellowCount = 0;

  // statistics of the green color
  unsigned int noGreenSeen=0; // pixel count where continously no green was seen
  unsigned int greenCount = 0;
  unsigned int greenValue = 0;
  int calculatedGreenValue = 0;


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
  Edgel edgel;

  for(int i = 1; i < scanLine.numberOfPixels; i++ )
  {
    const Pixel pixel = getImage().get(point.x,point.y);
    const int thisPixelBrightness = pixel.y;
    const ColorClasses::Color thisPixelColor = getColorTable64().getColorClass(pixel);

    DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:scanlines",
      POINT_PX(thisPixelColor, point.x, point.y);
    );
    DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:scanlines_noisy",
      int b_offset = thisPixelBrightness / 10;
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
      noGreenSeen = 0;
      greenCount++;
      greenValue += thisPixelBrightness;
      lastGreenPoint = point;

      whiteCount = 0;
      skyblueCount = 0;
      yellowCount = 0;
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
    }

    // scanline outside field / obstacle / check
    if
    (
      (lineBeginFound && noGreenSeen > MAX_POSSIBLE_LINE_THICKNESS) || // within a line
      (!lineBeginFound && noGreenSeen > 20 && noGreenSeen > 2 * whiteCount)// on the ground
    )
    {
      break;
    }

    // calculate the mean green value
    if (greenCount > 5)
    {
      calculatedGreenValue = (int)((greenValue / greenCount) + 0.5);
    }

    last_point = point;
    lastPixelBrightnessDiff = thisPixelBrightness - lastPixelBrightness;
    lastPixelBrightness = thisPixelBrightness;

    if(lastPixelBrightnessDiff < lastMinPixelBrightnessDiff)
    {
      lastMinPixelBrightnessDiff = lastPixelBrightnessDiff;
      last_min_point = point;
    }

    scanLine.getNext(point);
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
  ColorClasses::Color maxLastColor = ColorClasses::none;
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
Edgel ScanLineEdgelDetector::getEdgel(const Vector2<int>& start, const Vector2<int>& end, ScanLineState& scanLineState)
{
  Edgel edgel; //result
  unsigned int greenValue = 0;
  unsigned int greenCount = 0;
  unsigned int calculatedGreenValue = 0;

  unsigned int noGreenSeen = 0; // pixel count where continously no green was seen

  bool lineBeginRaiseFound = false;
  bool lineBeginFallFound = false;
  bool lineEndRaiseFound = false;
  bool lineEndFallFound = false;
  unsigned int thisPixelBrightness = 128;
  ColorClasses::Color thisPixelColor = ColorClasses::none;

  double maxSlope = SLOPE_THRESHOLD;
  double minSlope = -SLOPE_THRESHOLD;

  unsigned int slopeRaiseCount = 0;
  unsigned int slopeFallCount = 0;


  Vector2<int> point(start);

  BresenhamLineScan scanLine(start, end);

  for(int i = 0; i < scanLine.numberOfPixels; i++)
  {
    //scanline outside field / obstacle / check
    if (noGreenSeen > MAX_POSSIBLE_LINE_THICKNESS)
    {
      if(!getFieldPercept().getLargestValidRect(getCameraMatrix().horizon).isInside(point))
      {
        break;
      }
      //noGreenSeen = 0;
    }

    Pixel pixel = getImage().get(point.x,point.y);
    scanLineState.addBrightDiff((double) pixel.y - (double) thisPixelBrightness);
    thisPixelBrightness = pixel.y;
    thisPixelColor = getColorTable64().getColorClass(pixel);

    double slope = scanLineState.getSlope();

    DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:scanlines",
      POINT_PX(thisPixelColor, point.x, point.y);
    );
    DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:scanlines_noisy",
      int b_offset = thisPixelBrightness / 10;
      POINT_PX(thisPixelColor, point.x + b_offset, point.y);
    );

    if (greenCount > 5)
    {
      calculatedGreenValue = (int)((greenValue / greenCount) + 0.5);
    }

    if (!lineBeginFallFound)
    {

      if
      (
        slope > SLOPE_THRESHOLD &&
        greenCount > 5 &&
        (unsigned int) abs((int)(thisPixelBrightness -  calculatedGreenValue)) > edgelGrayThresholdLevel &&
        thisPixelColor != ColorClasses::green
      )
      {
        lineBeginRaiseFound = true;
        if(slope > maxSlope)
        {
          maxSlope = slope;
        }
        slopeRaiseCount++;
      }

      if(lineBeginRaiseFound && slope < SLOPE_THRESHOLD)
      {
        lineBeginFallFound = true;
      }

      //line beginning was found
      if(lineBeginFallFound)
      {
        for(unsigned int r = 0; r < (slopeRaiseCount / 2) + 1; r++)
        {
          scanLine.getLast(point);
        }
        edgel.begin = point;
        edgel.begin_angle = Math::normalizePositive(getPointsAngle(edgel.begin));
        for(unsigned int r = 0; r < slopeRaiseCount / 2; r++)
        {
          scanLine.getNext(point);
        }
        slopeRaiseCount = 0;
        maxSlope = SLOPE_THRESHOLD;
        noGreenSeen = 1;        
      }
      else
      {
        if (thisPixelColor == ColorClasses::green)
        {
          noGreenSeen = 0;
          greenCount++;
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
      if(!lineEndRaiseFound)
      {
        //searching for line end
        if
        (
          (
            slope < -SLOPE_THRESHOLD &&
            (unsigned int) abs((int)(thisPixelBrightness -  calculatedGreenValue)) > edgelGrayThresholdLevel &&
            thisPixelColor != ColorClasses::green
          )
          ||
          thisPixelColor == ColorClasses::green
        )
        {
          if(slope < minSlope)
          {
            minSlope = slope;
          }
          lineEndFallFound = true;
          slopeFallCount++;
        }

        if
        (
          lineEndFallFound &&
          (
            slope >= -SLOPE_THRESHOLD
            ||
            (unsigned int) abs((int)(thisPixelBrightness -  calculatedGreenValue)) <= edgelGrayThresholdLevel
            ||
            thisPixelColor == ColorClasses::green
          )
        )
        {
          lineEndRaiseFound = true;
        }

        if(lineEndRaiseFound)
        {

          for(unsigned int r = 0; r < (slopeFallCount / 2); r++)
          {
            scanLine.getLast(point);
          }
          edgel.end = point;
          edgel.end_angle = Math::normalizePositive(getPointsAngle(edgel.end));
          edgel.center = (edgel.end + edgel.begin) / 2;
          edgel.center_angle = (edgel.begin_angle + edgel.end_angle) / 2;
          evaluateEdgelThickness(edgel);
          for(unsigned int r = 0; r < slopeFallCount / 2; r++)
          {
            scanLine.getNext(point);
          }
          slopeFallCount = 0;
          minSlope = -SLOPE_THRESHOLD;

          noGreenSeen = 0;
          lineBeginRaiseFound = false;
          lineBeginFallFound = false;

          //both sobel angles should be somehow parallel
          if
          (
              fabs(edgel.begin_angle - edgel.center_angle) >= EDGEL_ANGLE_THRESHOLD &&
              fabs(edgel.end_angle - edgel.center_angle) >= EDGEL_ANGLE_THRESHOLD
          )
          {
            edgel.valid = false;
          }
          else
          {
            edgel.valid = true;
            break;
          }
          //break;
        }
      }
      noGreenSeen++;
    }//end else

    if(thisPixelColor == ColorClasses::green || thisPixelColor == ColorClasses::orange) // ignore the ball
    {
      scanLineState.lastGreenPoint = point;
      scanLineState.whiteCount = 0;
      scanLineState.skyblueCount = 0;
      scanLineState.yellowCount = 0;
    }
    else
    {
      // count the colors and remember the points
      if(thisPixelColor == ColorClasses::white)
      {
        scanLineState.whiteCount++;
        scanLineState.lastWhitePoint = point;
      }
      if(thisPixelColor == ColorClasses::skyblue)
      {
        scanLineState.skyblueCount++;
        scanLineState.lastSkyBluePoint = point;
      }
      if(thisPixelColor == ColorClasses::yellow)
      {
        scanLineState.yellowCount++;
        scanLineState.lastYellowPoint= point;
      }
    }
    scanLine.getNext(point);
  }//end for

  scanLineState.greenCount += greenCount;
  scanLineState.noGreenSeen += noGreenSeen;

  return edgel;
}//end getEdgel

void ScanLineEdgelDetector::getScanlineEndPoint(ScanLineState& scanLineState)
{
  scanLineState.endPoint.posInImage = scanLineState.lastGreenPoint;

  // determine the maximum off-green color
  ColorClasses::Color maxLastColor = ColorClasses::none;
  if(scanLineState.skyblueCount > scanLineState.whiteCount && scanLineState.skyblueCount > scanLineState.yellowCount)
  {
    if(scanLineState.skyblueCount > 5)
    {
      maxLastColor = ColorClasses::skyblue;
    }
  }
  else
  {
    if(scanLineState.yellowCount >= scanLineState.whiteCount)
    {
      if(scanLineState.yellowCount > 5)
      {
        maxLastColor = ColorClasses::yellow;
      }
    }
    else
    {
      if(scanLineState.whiteCount > 5)
      {
        maxLastColor = ColorClasses::white;
//        if(scanLineState.greenCount == 0)
//        {
//          scanLineState.endPoint.posInImage = scanLineState.lastWhitePoint;
//        }
      }
    }
  }//end else

  scanLineState.endPoint.color = maxLastColor;
}

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
      Math::clamp(x, 1, (signed int)getImage().cameraInfo.resolutionWidth-1);
      Math::clamp(y, 1, (signed int)getImage().cameraInfo.resolutionHeight-1);
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

void ScanLineEdgelDetector::evaluateEdgelThickness(Edgel& edgel)
{
  Vector2<double> d = edgel.end - edgel.begin;

  if(edgel.center_angle > Math::pi2)
  {
    edgel.thickness = d.rotate(edgel.center_angle).abs();
  }
  else if(edgel.center_angle < Math::pi2)
  {
    edgel.thickness = d.rotate(edgel.center_angle - Math::pi).abs();
  }
  else
  {
    edgel.thickness = d.abs();
  }
}//end evaluateEdgelThickness

double ScanLineEdgelDetector::calculateMeanAngle(double angle1,double angle2)
{
  //calculate unit vectors for both angles and add them
  double x=cos(2*angle1)+cos(2*angle2);
  double y=sin(2*angle1)+sin(2*angle2);

  //calculate sum vectors angle
  return(atan2(y, x)/2);
}//end calculateMeanAngle

void ScanLineEdgelDetector::drawDebug()
{
  DEBUG_REQUEST("ImageProcessor:ScanLineEdgelDetector:mark_end_points",
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

}//end drawDebug
