/**
* @file RobotDetector.cpp
*
* @author Kirill Yasinovskiy
* Implementation of class RobotDetector
*/

#include "RobotDetector.h"
#include "RobotDetectorFunctions.h"
//Debug
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"

RobotDetector::RobotDetector()
  : theBlobFinder(getColoredGrid())
{
  //initialize colors for blob finder
  //red:
  for (int n = 0; n < ColorClasses::numOfColors; n++)
  {
    redColors[n] = false;
  }
  redColors[ColorClasses::red] = true;
  //blue:
  for (int n = 0; n < ColorClasses::numOfColors; n++)
  {
    blueColors[n] = false;
  }
  blueColors[ColorClasses::blue] = true;
  blueColors[ColorClasses::skyblue] = true;
  //red and blue:
  for (int n = 0; n < ColorClasses::numOfColors; n++)
  {
    searchColors[n] = false;
  }
  searchColors[ColorClasses::red] = true;
  searchColors[ColorClasses::blue] = true;
  
  blueMarkers.reserve(MAX_MARKER_NUMBER);
  redMarkers.reserve(MAX_MARKER_NUMBER);
  resolutionWidth = getImage().cameraInfo.resolutionWidth;
  resolutionHeight = getImage().cameraInfo.resolutionHeight;

  DEBUG_REQUEST_REGISTER("ImageProcessor:RobotDetector:draw_blobs", "draw the blobs", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:RobotDetector:draw_poly", "draw marker's polygon", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:RobotDetector:draw_poly_params", "draw marker's polygon parameters", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:RobotDetector:draw_scanlines_marker", "draw marker scanlines", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:RobotDetector:draw_scanlines_robot", "draw robot scanlines", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:RobotDetector:draw_search_region", "draw blob search region", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:RobotDetector:draw_point_on_field", "draw the lowest robot's point", false);
}

void RobotDetector::execute()
{  
  searchArea.clear();
  blueBlobs.reset();
  redBlobs.reset();
  getPlayersPercept().reset();
  blueMarkers.clear();
  redMarkers.clear();
  //estimate the search region
  Vector2<int> p1(getCameraMatrix().horizon.begin());
  Vector2<int> p2(getCameraMatrix().horizon.end());
  if (p1.x < 0)
  {
    p1.x = 0;
  }
  if (p1.y < 0)
  {
    p1.y = 0;
  }
  searchArea.add(p1);
  searchArea.add(p2);
  searchArea.add(Vector2<int>(getImage().cameraInfo.resolutionWidth, getImage().cameraInfo.resolutionHeight));
  searchArea.add(Vector2<int>(0, getImage().cameraInfo.resolutionHeight));

  DEBUG_REQUEST("ImageProcessor:RobotDetector:draw_search_region",
    IMAGE_DRAWING_CONTEXT;
  RECT_PX(ColorClasses::yellow, searchArea.points[0].x, searchArea.points[0].y, searchArea.points[2].x, searchArea.points[2].y);
  );
  detectRobotMarkers();
  detectRobots();
}

void RobotDetector::detectRobots()
{
  for (std::vector<Marker>::const_iterator  iter = blueMarkers.begin(); iter != blueMarkers.end(); ++iter)
  {
    Marker marker = *iter;
    //if the marker's environment is "white enough"
    //we except it as a robot
    if(evaluateMarkerEnvironment(marker))
    {
      PlayersPercept::Player player;
      Vector2<int> lowestPoint((int)marker.cog.x, (int)marker.cog.y);
      int xStart((int)marker.cog.x - (int)marker.majorAxis.x);
      int xEnd((int)marker.cog.x + (int)marker.majorAxis.x);
      int stepSize(2);
      //now we want to estimate the robot's lowest point
      while ((unsigned int)lowestPoint.y < getImage().cameraInfo.resolutionHeight
             && findGreenRatio(lowestPoint.y, xStart, xEnd, stepSize) < GREEN_GROUND_RATIO)
      {
        lowestPoint.y += stepSize;
      }
      //is the robot standing? 
      bool standing = ((marker.angle >= 0.0f && marker.angle <= 0.5f) || (marker.angle >= Math::pi - 0.5f && marker.angle <= Math::pi)) ? true : false;
      //estimate the position of the robot based on lowest point
      Vector2<double> positon;
      Vector2<double> angleTo;
      Geometry::imagePixelToFieldCoord(getCameraMatrix(), getImage().cameraInfo, lowestPoint.x, lowestPoint.y, 0.0, positon);
      angleTo = Geometry::angleToPointInImage(getCameraMatrix(), getImage().cameraInfo, marker.cog.x, marker.cog.y);
      player.pose.translation = positon;
      player.angleTo = angleTo;
      player.isStanding = standing;
      player.teamColor = marker.color;
      //add robot into playersPercept
      //if the lowest point lies over the imageboundaries
      if (exactLocalisation(getImage(), lowestPoint))
      {
        getPlayersPercept().addPlayer(player);
      }
      else
      {
        getPlayersPercept().addUnlocalizedPlayer(player);
      }
      
      //debug
      DEBUG_REQUEST("RobotDetector:draw_point_on_field",
        RECT_PX(ColorClasses::green, lowestPoint.x-2,lowestPoint.y-2,lowestPoint.x+2,lowestPoint.y+2);
        POINT_PX(ColorClasses::yellow, lowestPoint.x, lowestPoint.y);
        );
    }
  }
}

void RobotDetector::detectRobotMarkers()
{
  findBlobs();
  for (int i = 0; i < blueBlobs.blobNumber; i++)
  {
    //create temp blob
    Blob blob = blueBlobs.blobs[i];
    Vector2<double> cog = blob.moments.getCentroid();
    double mom = blob.moments.getRawMoment(0,0);
    if (mom > BLOB_MIN_MOMENT)
    {
      findMarkerPoly(cog, ColorClasses::blue);
    }
  }
  for (int i = 0; i < redBlobs.blobNumber; i++)
  {
    //create temp blob
    Blob blob = redBlobs.blobs[i];
    Vector2<int> cog = blob.moments.getCentroid();
    double mom = blob.moments.getRawMoment(0,0);
    if (mom > BLOB_MIN_MOMENT)
    {
      findMarkerPoly(cog, ColorClasses::red);
    }
  }
}//end detectRobotMarkers

bool RobotDetector::evaluateMarkerEnvironment(Marker& marker)
{
  bool result = true;
  drawScanLinesRobot = false;
  DEBUG_REQUEST("ImageProcessor:RobotDetector:draw_scanlines_robot",
    drawScanLinesRobot = true;
  );
  Vector2<int> left = marker.cog - marker.majorAxis;
  Vector2<int> right = marker.cog + marker.majorAxis;
  Vector2<int> middle = marker.cog;
  //check the marker's environment above the marker
  Vector2<int> leftUp = left - marker.minorAxis*4;
  Vector2<int> middleUp = marker.cog - marker.minorAxis*4;
  Vector2<int> rightUp = right - marker.minorAxis*4;
  int whitePixelsAbove = 0;
  int totalPixelsAbove = 0;
  totalPixelsAbove += scanline(getImage(), getColorTable64(), ColorClasses::white, left-marker.minorAxis, leftUp, whitePixelsAbove, drawScanLinesRobot); 
  totalPixelsAbove += scanline(getImage(), getColorTable64(), ColorClasses::white, middle-marker.minorAxis, middleUp, whitePixelsAbove, drawScanLinesRobot); 
  totalPixelsAbove += scanline(getImage(), getColorTable64(), ColorClasses::white, right-marker.minorAxis, rightUp, whitePixelsAbove, drawScanLinesRobot);
  double aboveWhiteRatio = (double)whitePixelsAbove/(double)totalPixelsAbove;

  //check the marker's environment below the marker
  Vector2<int> leftDown = left + marker.minorAxis*4;
  Vector2<int> middleDown = marker.cog + marker.minorAxis*2;
  Vector2<int> rightDown = right + marker.minorAxis*4;
  int whitePixelsBelow = 0;
  int totalPixelsBelow = 0;
  totalPixelsBelow += scanline(getImage(), getColorTable64(), ColorClasses::white, left+marker.minorAxis, leftDown, whitePixelsBelow, drawScanLinesRobot);
  totalPixelsBelow += scanline(getImage(), getColorTable64(), ColorClasses::white, middle+marker.minorAxis, middleDown, whitePixelsBelow, drawScanLinesRobot);
  totalPixelsBelow += scanline(getImage(), getColorTable64(), ColorClasses::white, right+marker.minorAxis, rightDown, whitePixelsBelow, drawScanLinesRobot);
  double belowWhiteRatio = (double)whitePixelsBelow/(double)totalPixelsBelow;  
  result = (belowWhiteRatio > BELOW_WHITE_RATIO && aboveWhiteRatio > ABOVE_WHITE_RATIO) ? true : false;
  return result;
}//end evaluateMarkerEnvironment

void RobotDetector::findBlobs()
{  
  //start finding the blobs
  STOPWATCH_START("RobotDetector: find blobs");
  // find red blobs
  theBlobFinder.execute(redBlobs, redColors, searchArea);

  // find blue blobs
  theBlobFinder.execute(blueBlobs, blueColors, searchArea);
  STOPWATCH_STOP("RobotDetector: find blobs");
  
  // debug
  IMAGE_DRAWING_CONTEXT;
  DEBUG_REQUEST("ImageProcessor:RobotDetector:draw_blobs",
    for (int i = 0; i < redBlobs.blobNumber; i++)
    {
      Vector2<int> c = redBlobs.blobs[i].centerOfMass;
      Vector2<int>& p1 = redBlobs.blobs[i].upperLeft;
      Vector2<int>& p2 = redBlobs.blobs[i].lowerRight;
      RECT_PX(ColorClasses::red, p1.x, p1.y, p2.x, p2.y);
    }//end for

    for (int i = 0; i < blueBlobs.blobNumber; i++)
    {
      Vector2<int> c = blueBlobs.blobs[i].centerOfMass;
      Vector2<int>& p1 = blueBlobs.blobs[i].upperLeft;
      Vector2<int>& p2 = blueBlobs.blobs[i].lowerRight;
      RECT_PX(ColorClasses::blue, p1.x, p1.y, p2.x, p2.y);
    }//end for
    );//end debug
}// end findBlobs

void RobotDetector::findMarkerPoly(Vector2<int> cog, ColorClasses::Color color)
{
  drawScanLinesMarker = false;
  DEBUG_REQUEST("ImageProcessor:RobotDetector:draw_scanlines_marker",
              drawScanLinesMarker = true;
  );
  
  STOPWATCH_START("RobotDetector:find poly");

  Marker marker;
  // scan directions
  Vector2<int> up(0, -1);
  Vector2<int> upUpRight(1, -2);
  Vector2<int> upRight(1, -1);
  Vector2<int> upRightRight(2, -1);
  Vector2<int> right(1, 0);
  Vector2<int> downRightRight(2, 1);
  Vector2<int> downRight(1, 1);
  Vector2<int> downDownRight(1, 2);
  Vector2<int> down(0, 1);
  Vector2<int> downDownLeft(-1, 2);
  Vector2<int> downLeft(-1, 1);
  Vector2<int> downLeftLeft(-2, 1);
  Vector2<int> left(-1, 0);
  Vector2<int> upLeftLeft(-2, -1);
  Vector2<int> upLeft(-1, -1);
  Vector2<int> upUpLeft(-1, -2);
  // temp point to store results
  Vector2<int> Point;
  Vector2<int> firstPoint;

  //scan in all directions
  //TODO: make functions

  scanLine(cog, up, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  firstPoint = Point;
  scanLine(cog, upUpRight, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, upRight, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, upRightRight, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, right, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, downRightRight, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, downRight, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, downDownRight, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, down, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, downDownLeft, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, downLeft, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, downLeftLeft, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, left, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, upLeftLeft, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, upLeft, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);
  scanLine(cog, upUpLeft, 4, color, Point, drawScanLinesMarker);
  marker.polygon.add(Point);

  //add the firstPoint to complete the Polygon
  marker.polygon.add(firstPoint);

  //add the color
  if (color == ColorClasses::blue)
  {
    marker.color = GameData::blue;
  }
  if (color == ColorClasses::red)
  {
    marker.color = GameData::red;
  }
 
  // if the marker is good enough, 
  // take him
  if (checkMarkerPoly(marker))
  {
    if(color == ColorClasses::blue)
    blueMarkers.push_back(marker);
    if(color == ColorClasses::red)
    redMarkers.push_back(marker);
  }
  
  STOPWATCH_STOP("RobotDetector:find poly");
  DEBUG_REQUEST("RobotDetector:draw_poly",
              IMAGE_DRAWING_CONTEXT;
  for (std::vector<Marker>::const_iterator iter = blueMarkers.begin(); iter != blueMarkers.end(); ++iter)
  {
    Vector2<int> p1 = iter->polygon.points[0];
    for (int i = 1; i < iter->polygon.length; i++)
    {
      Vector2<int> p2 = iter->polygon.points[i];
      LINE_PX(ColorClasses::white, p1.x, p1.y, p2.x, p2.y);
      p1 = p2;
    }//end for
  }//end for
  for (std::vector<Marker>::const_iterator iter = redMarkers.begin(); iter != redMarkers.end(); ++iter)
  {
    Vector2<int> p1 = iter->polygon.points[0];
    for (int i = 1; i < iter->polygon.length; i++)
    {
      Vector2<int> p2 = iter->polygon.points[i];
      LINE_PX(ColorClasses::white, p1.x, p1.y, p2.x, p2.y);
      p1 = p2;
    }//end for
  }//end for
  );//end debug
}//end findmarkerpoly


//check marker's properties
bool RobotDetector::checkMarkerPoly(Marker& marker)
{
  STOPWATCH_START("RobotDetector: check poly params");

  bool result;
  // get the marker's area
  estimateArea(marker);
  // if the marker too small,
  // don't take him
  if (marker.area < MARKER_MIN_SIZE)
  {
    result = false;
  } 
  else
  { 
    // the marker is big enough, we gonna check
    // his parameters
    
    estimateMoments(marker);
    STOPWATCH_STOP("RobotDetector: check poly params");

    result = true;
    if (1.5 < marker.eccentricity && marker.eccentricity < 13)
    {
      result = true;
    } 
    else
    {
      result = false;
    }
    // debug
    DEBUG_REQUEST("ImageProcessor:RobotDetector:draw_poly_params",
      POINT_PX(ColorClasses::yellow, (int)marker.cog.x, (int)marker.cog.y);
      LINE_PX(ColorClasses::gray,(int)(marker.cog.x - marker.majorAxis.x), (int)(marker.cog.y - marker.majorAxis.y), (int)(marker.cog.x + marker.majorAxis.x), (int)(marker.cog.y + marker.majorAxis.y));
      LINE_PX(ColorClasses::black,(int)(marker.cog.x - marker.minorAxis.x), (int)(marker.cog.y - marker.minorAxis.y), (int)(marker.cog.x + marker.minorAxis.x), (int)(marker.cog.y + marker.minorAxis.y));

    );//end debug
  }
  return result;
}//end checkMarkerPoly

//help functions
//to validate the polygon

void RobotDetector::estimateArea(Marker& marker)
{
  int area = 0;
  for (int n = 1; n <= marker.polygon.length; n++)
  {
    area += (marker.polygon.points[n-1].x * marker.polygon.points[n].y - 
            marker.polygon.points[n].x * marker.polygon.points[n-1].y);
  }
  marker.area = abs(area/2);
}

void RobotDetector::estimateMoments(Marker& marker)
{
  int tempValue = 0;
  double normalizedMoment10 = 0;
  double normalizedMoment01 = 0;
  double normalizedMoment20 = 0;
  double normalizedMoment02 = 0;
  double normalizedMoment11 = 0;
  
  for (int n = 1; n <= marker.polygon.length; n++)
  {
    int xi_1 = marker.polygon.points[n-1].x;
    int xi = marker.polygon.points[n].x;
    int yi_1 = marker.polygon.points[n-1].y;
    int yi = marker.polygon.points[n].y;

    tempValue = (xi_1 * yi - xi * yi_1);
    normalizedMoment10 += ((tempValue) * (xi_1 + xi));
    normalizedMoment01 += ((tempValue) * (yi_1 + yi));
    normalizedMoment20 += ((tempValue) * (xi_1 * xi_1 + xi_1 * xi + xi * xi));
    normalizedMoment02 += ((tempValue) * (yi_1 * yi_1 + yi_1 * yi + yi * yi));
    normalizedMoment11 += ((tempValue) * (2*xi_1*yi_1 + xi_1*yi + xi*yi_1 + 2*xi*yi)); 
  }
  normalizedMoment10 = normalizedMoment10/(6*marker.area);
  normalizedMoment01 = normalizedMoment01/(6*marker.area);
  normalizedMoment20 = normalizedMoment20/(12*marker.area);
  normalizedMoment02 = normalizedMoment02/(12*marker.area);
  normalizedMoment11 = normalizedMoment11/(24*marker.area);

  double centralMoment20 = (normalizedMoment20 - normalizedMoment10 * normalizedMoment10);
  double centralMoment02 = (normalizedMoment02 - normalizedMoment01 * normalizedMoment01);
  double centralMoment11 = (normalizedMoment11 - normalizedMoment10 * normalizedMoment01);

  double sum = (centralMoment20 + centralMoment02);
  double diff = (centralMoment20 - centralMoment02);
  double eccroot = sqrt(diff*diff + 4*centralMoment11*centralMoment11);

  double largerEigenValue = sum + eccroot;
  double smallerEigenValue = sum - eccroot;

  marker.eccentricity = largerEigenValue/smallerEigenValue;

  marker.cog.x = normalizedMoment10;
  marker.cog.y = normalizedMoment01;

  marker.angle = 0.5*atan2(2*centralMoment11, centralMoment20-centralMoment02);
  
  marker.majorAxis.x = cos(marker.angle);
  marker.majorAxis.y = sin(marker.angle);
  marker.minorAxis.x = -marker.majorAxis.y;
  marker.minorAxis.y = marker.majorAxis.x;

  //scale the axes
  marker.majorAxis *= sqrt(largerEigenValue);
  marker.minorAxis *= sqrt(smallerEigenValue);
}

double RobotDetector::findGreenRatio(int yCoord, int xStart, int xEnd, int stepSize)
{
  const double numberOfPixels(double(xEnd - xStart)/stepSize);
  int greenColor(0);
  for (int i = xStart; i <= xEnd; i+=stepSize)
  {
    Pixel pixel = getImage().get(i, yCoord);
    ColorClasses::Color currentPixelColor = getColorTable64().getColorClass(pixel);
    if(currentPixelColor == ColorClasses::green)
    {
      greenColor++;
    }
  }
  return static_cast<double> (greenColor) / numberOfPixels;
}

// scan section

void RobotDetector::scanLine(Vector2<int> start, Vector2<int>& direction, int maxColorPointsToSkip, ColorClasses::Color searchColor, Vector2<int>& point, bool draw)
{
  Vector2<int> currentPoint(start);  //set the starting point
  int searchColorPointsSkipIndex(0);  //reset number of skipped pixels
  Vector2<int> borderPoint;      // to remember the border point ...
  bool borderPointFound = false; //if one was found
  Vector2<int> lastSearchColorPoint(start);
  unsigned int max_length_of_beam(100);

  //expand in the selected direction
  for(unsigned int j = 0; j < max_length_of_beam; j++)
  {
  // check the current point:
  // we have already too many pixels, that don't have right color,
  // or the pixel isn't in search area
    if( searchColorPointsSkipIndex > maxColorPointsToSkip ||
      !pixelInSearchArea(currentPoint))
    {
      break;
    }//end if

  ////////////////////////////////
  // process the current point
  ////////////////////////////////

  Pixel pixel = getImage().get(currentPoint.x,currentPoint.y);
  ColorClasses::Color currentPixelColor = getColorTable64().getColorClass(pixel);
  //check whether the current point has the right color
  bool hasColor = isSearchColor(currentPixelColor, searchColor);
  if(hasColor)
  {
    lastSearchColorPoint = currentPoint;
    borderPoint = currentPoint;
    searchColorPointsSkipIndex = 0;
    borderPointFound = true;
  }//end else
  else
  {
    searchColorPointsSkipIndex++;
  }//end else
  if(draw)
  {
    if(hasColor)
    {
      if(currentPixelColor == ColorClasses::none)
      {
        POINT_PX(ColorClasses::orange, (unsigned int)(currentPoint.x), (unsigned int)(currentPoint.y));
      }
      else
      {
        POINT_PX(ColorClasses::green, (unsigned int)(currentPoint.x), (unsigned int)(currentPoint.y));
      }
    }
    else
    {
      POINT_PX(ColorClasses::red, (unsigned int)(currentPoint.x), (unsigned int)(currentPoint.y));
    }
  }
  currentPoint += direction;
  }//end for

if(borderPointFound) //if a point was found ...
  {
    point = borderPoint;
    //goodPoints.add(borderPoint); //it's good point
  }//end if
}//end scanLine


//check whether a point is in the image
bool RobotDetector::pixelInSearchArea(Vector2<int>& pixel)
  {
  return (pixel.x >= searchArea[0].x && pixel.y >= searchArea[0].y 
    &&(unsigned int)pixel.x<resolutionWidth &&
    (unsigned int)pixel.y<resolutionHeight);
  }//end pixelInImage


bool RobotDetector::isSearchColor(ColorClasses::Color color, ColorClasses::Color searchColor)
{
  if(color == searchColor)
  {
    return true;
  }
  return false;
}
