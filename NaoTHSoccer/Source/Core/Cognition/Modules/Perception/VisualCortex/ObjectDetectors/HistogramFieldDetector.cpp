
/**
* @file HistogramFieldDetector.cpp
*
* Implementation of class HistogramFieldDetector
*
*/

#include "HistogramFieldDetector.h"

HistogramFieldDetector::HistogramFieldDetector()
{
  fieldColor = ColorClasses::green;
  lineColor = ColorClasses::white;

  DEBUG_REQUEST_REGISTER("ImageProcessor:HistogramFieldDetector:mark_rectangle", "mark boundary rectangle of the detected field on the image", false);

  getFieldPercept().setDimension(Vector2<int>(getImage().cameraInfo.resolutionWidth, getImage().cameraInfo.resolutionHeight));
}

void HistogramFieldDetector::execute()
{
  getFieldPercept().reset();
  largestAreaRectangle.clear();

  Vector2<int> min(0,0);
  Vector2<int> max(0,0);

  getFieldRectFromHistogram(min, max);

  getFieldPercept().setRect(largestAreaRectangle, getArtificialHorizon());

  //if die enclosured area of the polygon/rectangle is lower than 11200 squarepixels the area is to small
  //TODO: this could be an topic of some kind of learning
  if(largestAreaRectangle.getArea() >= 5600)
  {
    getFieldPercept().setValid(true);
  }
  DEBUG_REQUEST( "ImageProcessor:HistogramFieldDetector:mark_rectangle",
    ColorClasses::Color color = getFieldPercept().isValid() ? ColorClasses::green : ColorClasses::red;
      RECT_PX
      (
          color,
          getFieldPercept().getLargestValidRect().points[0].x,
          getFieldPercept().getLargestValidRect().points[0].y,
          getFieldPercept().getLargestValidRect().points[2].x,
          getFieldPercept().getLargestValidRect().points[2].y
      );
  );
}//end execute

void HistogramFieldDetector::getFieldRectFromHistogram(Vector2<int>& min, Vector2<int>& max)
{
  Vector2<int> actMin(-1,-1);
  Vector2<int> actMax(-1,-1);
  const int minXHistLevel = (int) (getColoredGrid().uniformGrid.width * 0.10);
  const int minYHistLevel = (int) (getColoredGrid().uniformGrid.height * 0.10);

  int whiteCount = 0;
  int otherCount = 0;

  //go through histogram for values along the y axis and pick the largest area of green color as partial field y axis
  for (unsigned int y = 0; y < getColoredGrid().uniformGrid.height; y++)
  {
    if
    (
      (
      getHistogram().xHistogram[fieldColor][y] >= minXHistLevel
      || 0.9 * getHistogram().xHistogram[fieldColor][y] + 0.1 * getHistogram().xHistogram[lineColor][y] >= minXHistLevel
      )
      && whiteCount <= LINE_THICKNESS
    )
    {
      otherCount = 0;
      if(getHistogram().xHistogram[fieldColor][y] >= minXHistLevel)
      {
        if(whiteCount > 0)
        {
          whiteCount = 0;
        }
      }
      if(actMin.y == -1)//actMax.y)
      {
        actMin.y = y;
        actMax.y = y;
      }
      else
      {
        actMax.y = y;
      }
    }
    else if(getHistogram().xHistogram[lineColor][y] >= minXHistLevel && (actMax.y - actMin.y) > 1)
    {
      whiteCount++;
      otherCount = 0;
    }
  else
  {
      if(otherCount > 2)
      {
        if( (actMax.y - actMin.y) >= (max.y - min.y) )
        {
          min.y = actMin.y;
          max.y = actMax.y;
          whiteCount = 0;
        }
        actMin.y = -1;
        actMax.y = -1;
      }
      otherCount++;
    }
  }
  if( (actMax.y - actMin.y) >= (max.y - min.y) )
  {
    min.y = actMin.y;
    max.y = actMax.y;
  }

  //go through histogram for values along the x axis and pick the largest area of green color as partial field x axis
  for (unsigned int x = 0; x < getColoredGrid().uniformGrid.width; x++)
  {
    if
    (
      (
      getHistogram().yHistogram[fieldColor][x] >= minYHistLevel
      || 0.9 * getHistogram().yHistogram[fieldColor][x] + 0.1 * getHistogram().yHistogram[lineColor][x] >= minYHistLevel
      )
    )
    {
      if(actMin.x == -1)//actMax.x)
      {
        actMin.x = x;
        actMax.x = x;
      }
      else
      {
        actMax.x = x;
      }
    }
    else
    {
      if( (actMax.x - actMin.x) >= (max.x - min.x) )
      {
        min.x = actMin.x;
        max.x = actMax.x;
      }
//      actMin.x = -1;
//      actMax.x = -1;
    }
  }
  if( (actMax.x - actMin.x) >= (max.x - min.x) )
  {
    min.x = actMin.x;
    max.x = actMax.x;
  }

  if(min.x > -1 && min.x > -1 && max.x > -1 && max.y > -1)
  {
    largestAreaRectangle.add(getColoredGrid().getImagePoint(min));
    Vector2<int> v1(min.x,max.y);
    largestAreaRectangle.add(getColoredGrid().getImagePoint(v1));
    largestAreaRectangle.add(getColoredGrid().getImagePoint(max));
    Vector2<int> v2(max.x,min.y);
    largestAreaRectangle.add(getColoredGrid().getImagePoint(v2));
  }

}//end getFieldRectFromHistogram
