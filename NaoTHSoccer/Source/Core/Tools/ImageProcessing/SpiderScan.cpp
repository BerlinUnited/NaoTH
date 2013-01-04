/**
* @file Tools/ImageProcessing/SpiderScan.cpp
* 
* Utility class which performs a SpiderScan algorithm for blob region scanning
*
*/

#include "SpiderScan.h"


SpiderScan::SpiderScan(const Image& theImage, const ColorClassificationModel& theColorClassifier)
:
theImage(theImage),
theColorClassifier(theColorClassifier)
{
  searchColors = vector<ColorClasses::Color>(1, ColorClasses::numOfColors);
  borderColors = vector<ColorClasses::Color>(1, ColorClasses::green);
  init();
}

SpiderScan::SpiderScan(const Image& theImage, const ColorClassificationModel& theColorClassifier, ColorClasses::Color searchColor)
:
theImage(theImage),
theColorClassifier(theColorClassifier)
{
  searchColors = vector<ColorClasses::Color>(1, searchColor);
  borderColors = vector<ColorClasses::Color>(1, ColorClasses::numOfColors);
  init();
}

SpiderScan::SpiderScan(const Image& theImage, const ColorClassificationModel& theColorClassifier, vector<ColorClasses::Color>& searchColors)
:
theImage(theImage),
theColorClassifier(theColorClassifier),
searchColors(searchColors)
{
  borderColors = vector<ColorClasses::Color>(1, ColorClasses::numOfColors);
  init();
}

SpiderScan::SpiderScan(const Image& theImage, const ColorClassificationModel& theColorClassifier, ColorClasses::Color searchColor, ColorClasses::Color borderColor)
:
theImage(theImage),
theColorClassifier(theColorClassifier)
{
  searchColors = vector<ColorClasses::Color>(1, searchColor);
  borderColors = vector<ColorClasses::Color>(1, borderColor);
  init();
}

SpiderScan::SpiderScan(const Image& theImage, const ColorClassificationModel& theColorClassifier, vector<ColorClasses::Color>& searchColors, vector<ColorClasses::Color>& borderColors)
:
theImage(theImage),
theColorClassifier(theColorClassifier),
searchColors(searchColors),
borderColors(borderColors)
{
  init();
}

bool SpiderScan::isBorderColor(ColorClasses::Color color) const 
{
  for(unsigned int i = 0; i < borderColors.size(); i++)
  {
    if(color == borderColors[i])
    {
      return true;
    }
  }
  return false;
}//end isBorderColor

bool SpiderScan::isSearchColor(ColorClasses::Color color) const 
{
  for(unsigned int i = 0; i < searchColors.size(); i++)
  {
    if(color == searchColors[i])
    {
      return true;
    }
  }
  return false;
}//end isSearchColor

void SpiderScan::setMaxBeamLength(unsigned int length)
{
  max_length_of_beam = length;
}

void SpiderScan::setCurrentColorSimThreshold(double threshold)
{
  currentColorSimThreshold = threshold;
}

void SpiderScan::setMaxColorPointsToSkip(unsigned int maxToSkip)
{
  maxColorPointsToSkip = maxToSkip;
}

void SpiderScan::setMaxNumberOfScans(unsigned int maxScans)
{
  maxNumberOfScans = maxScans;
}

void SpiderScan::init()
{
  drawScanLines = false;
  max_length_of_beam = 30; //maximum length of the scan line
  currentColorSimThreshold = 8; //...
  maxColorPointsToSkip = 12; // maximum number of non search color ...
  maxNumberOfScans = 15; //maximum number of scanlines ...

}

void SpiderScan::setDrawScanLines(bool draw)
{
  drawScanLines = draw;
}


void SpiderScan::scan(const Vector2<int>& start, PointList<20>& goodPoints, PointList<20>& badPoints)
{
  Scans scans; //a list of scans to perform

  //add the standard scan lines
  scans.add(start, Vector2<int>( 0,-1));// down
  scans.add(start, Vector2<int>(-1,-1));// to the lower left
  scans.add(start, Vector2<int>(-1, 0));// to the left
  scans.add(start, Vector2<int>(-1, 1));// to the upper left
  scans.add(start, Vector2<int>( 0, 1));// up
  scans.add(start, Vector2<int>( 1, 1));// to the upper right
  scans.add(start, Vector2<int>( 1, 0));// to the right
  scans.add(start, Vector2<int>( 1,-1));// to the lower right
  
  scan(goodPoints, badPoints, scans);
}

void SpiderScan::scan(PointList<20>& goodPoints, PointList<20>& badPoints, Scans scans)
{
  //remember the number of performed scans
  int numberOfScans = 0;

  while(scans.number > 0 && numberOfScans < maxNumberOfScans) //while there are scans to perform (but not too much)
  {
    // ...
    numberOfScans++;
    //Vector2<int>& direction = scans.direction[0]; //get the direction of the scan line
//    Vector2<int> currentPoint(scans.start[0]);    //and the starting point

    int badPointsBeforeScan = badPoints.length; //remember the number of bad points (to see if one was added after the scan)

    //perform the scan, see if it was successful
    if(scanLine(scans.start[0], scans.direction[0], maxColorPointsToSkip, goodPoints, badPoints))
    {
      if(badPoints.length > badPointsBeforeScan) //if a bad point was found
      {
        const Vector2<int>& badPoint = badPoints[badPoints.length-1]; //get this point

        //if the pixel lies at the image border and was not the result of a scan along that border
        if(pixelAtImageBorder(badPoint, 2) && !isBorderScan(scans.start[0], scans.direction[0], 2))
        {
          if(badPoint.x <= 1 || (unsigned int)badPoint.x >= theImage.cameraInfo.resolutionWidth-2)
          {
            //... its not reliable and we should scan along the imageborder to find the real border point
            scans.add(badPoint, Vector2<int>(0,  1));
            scans.add(badPoint, Vector2<int>(0, -1));
          }
          //same for the vertical borders
          else if(badPoint.y <= 1 || (unsigned int)badPoint.y >= theImage.cameraInfo.resolutionHeight-2)
          {
            scans.add(badPoint, Vector2<int>( 1, 0));
            scans.add(badPoint, Vector2<int>(-1, 0));
          }

          badPoints.remove(badPoints.length-1); //kick the unreliable Pixel
        }//end if pixelAtImageBorder
      }//end if BadPointFound
    }//end if borderPointFound

    scans.remove(0); //scan done, remove it from the list
  }//end while scans left;

}//end spiderSearch

bool SpiderScan::scanLine(const Vector2<int>& start, const Vector2<int>& direction, int maxColorPointsToSkip, PointList<20>& goodPoints, PointList<20>& badPoints) const
{
  Vector2<int> currentPoint(start);  //set the starting point
  int searchColorPointsSkipIndex = 0;  //reset number of skipped pixels
  Vector2<int> borderPoint;      // to remember the border point ...
  bool borderPointFound = false; //if one was found
  bool borderPixelFound = false;      //and if it was followed by a border pixel

  Vector2<int> lastSearchColorPoint(-1,-1);

  //expand in the selected direction
  for(unsigned int j = 0; j < max_length_of_beam; j++)
  {

    // check the current point
    if( searchColorPointsSkipIndex > maxColorPointsToSkip ||
        // point outside the image
        !pixelInImage(currentPoint))
    {
      break;
    }//end if

    ////////////////////////////////
    // process the current point
    ////////////////////////////////

    Pixel pixel = theImage.get(currentPoint.x,currentPoint.y);
    ColorClasses::Color currentPixelColor = theColorClassifier.getColorClass(pixel);

//    bool hasColor = (currentPixelColor == searchColor) || (searchColor == ColorClasses::numOfColors && currentPixelColor != borderColor);
    bool hasColor = isSearchColor(currentPixelColor) || (searchColors[0] == ColorClasses::numOfColors && !isBorderColor(currentPixelColor));

    // HACK: taken from GT07
    // TODO: make it more general by taking the criteria out of the method
    if(!hasColor)
    {
      if(lastSearchColorPoint.x != -1 && lastSearchColorPoint.y != -1)
      {
        //hasColor = currentOrangeSim > currentColorSimThreshold;

        Pixel pixelLast = theImage.get(lastSearchColorPoint.x,lastSearchColorPoint.y);
        double yDiff = pixel.y - pixelLast.y;
        double uDiff = pixel.u - pixelLast.u;
        double vDiff = pixel.v - pixelLast.v;


        double useY = 0.0;
  //      MODIFY("ImageProcessor:Detector:currentColorSimThreshold", currentColorSimThreshold);
  //      MODIFY("ImageProcessor:Detector:useY", useY);

  //      hasColor =
  //        ((currentPixelColor == ColorClasses::none)  || (searchColor == ColorClasses::numOfColors && currentPixelColor != borderColor)) &&
  //        searchColorPointsSkipIndex == 0 &&
  //        sqrt( useY*Math::sqr(yDiff) + Math::sqr(uDiff) + Math::sqr(vDiff) ) < currentColorSimThreshold;
        hasColor =
          (currentPixelColor == ColorClasses::none || (searchColors[0] == ColorClasses::numOfColors &&  !isBorderColor(currentPixelColor))) &&
          searchColorPointsSkipIndex == 0 &&
          sqrt( useY*Math::sqr(yDiff) + Math::sqr(uDiff) + Math::sqr(vDiff) ) < currentColorSimThreshold;
      }//end if
    }
    else
    {
      lastSearchColorPoint = currentPoint;
    }

    if(hasColor)
    {
      borderPoint = currentPoint;
      searchColorPointsSkipIndex = 0;
      borderPointFound = true;
      borderPixelFound = false;
    }
    else
    {
//      if
//      (
//        currentPixelColor == borderColor ||
//        (borderColor == ColorClasses::numOfColors && currentPixelColor != searchColor)
////        (currentPixelColor != searchColor && borderColor == ColorClasses::numOfColors) ||
////        (searchColor == ColorClasses::numOfColors && currentPixelColor != borderColor)
//      )
      if
      (
        isBorderColor(currentPixelColor) ||
        (borderColors[0] == ColorClasses::numOfColors && !isSearchColor(currentPixelColor) )
      )
      {
        borderPixelFound = true;
      }
      searchColorPointsSkipIndex++;
    }//end if

    if(drawScanLines)
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
    //that point was followed by a border pixel, does not lie at the images border or is the result of a scan along that border ...
    if(borderPixelFound && (!pixelAtImageBorder(borderPoint, 2) || isBorderScan(borderPoint, direction, 2)))
    {
      goodPoints.add(borderPoint); //it's good point
    }
    else
    {
      badPoints.add(borderPoint);  //otherwise it's a bad one
    }
  }//end if

  return borderPointFound; //return if a borderpoint was found
}//end scanLine

//check wether a scan line goes along an image border
bool SpiderScan::isBorderScan(const Vector2<int>& point, const Vector2<int>& direction, int borderWidth) const
{
  return ((point.x<borderWidth || (unsigned int)point.x>=theImage.cameraInfo.resolutionWidth-borderWidth) && direction.x==0) ||
         ((point.y<borderWidth || (unsigned int)point.y>=theImage.cameraInfo.resolutionHeight-borderWidth) && direction.y==0);
}//end isBorderScan

//check wether a point is in the image
bool SpiderScan::pixelInImage(const Vector2<int>& pixel) const
{
  return (pixel.x >= 0 && pixel.y >= 0 &&
          (unsigned int)pixel.x<theImage.cameraInfo.resolutionWidth &&
          (unsigned int)pixel.y<theImage.cameraInfo.resolutionHeight);
}//end pixelInImage

//check wether a point lies at the images border
bool SpiderScan::pixelAtImageBorder(const Vector2<int>& pixel, int borderWidth) const
{
  return (pixel.x < borderWidth || pixel.y < borderWidth ||
          (unsigned int)pixel.x>=theImage.cameraInfo.resolutionWidth-borderWidth ||
          (unsigned int)pixel.y>=theImage.cameraInfo.resolutionHeight-borderWidth);
}//end pixelAtImageBorder
