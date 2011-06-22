/**
* @file RobotDetectorFunctions.h
*
* @author Kirill Yasinovskiy, based on B-Human source code
* Definition of class RobotDetector
*/

#ifndef __RobotDetectorFunctions_h_
#define __RobotDetectorFunctions_h_

#include "Tools/Math/Vector2.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Tools/Debug/DebugImageDrawings.h"


inline int scanline(
             const Image& theImage,
             const ColorTable64& theColorTable64,
             const ColorClasses::Color& teamColor,
             const Vector2<int>& start,
             Vector2<int>& end,
             int& whitePixels,
             bool draw)
{
  BresenhamLineScan scan = BresenhamLineScan(start, end);
  scan.init();
  end = start;
  for(int i = 0; i < scan.numberOfPixels; i++)
  {
    scan.getNext(end);
    Pixel pixel = theImage.get(end.x,end.y);
    ColorClasses::Color currentPixelColor = theColorTable64.getColorClass(pixel);
    if(end.x >= 0 && (unsigned int)end.x < theImage.cameraInfo.resolutionWidth
      && end.y >= 0 && (unsigned int)end.y < theImage.cameraInfo.resolutionHeight)
    {
      if(currentPixelColor == ColorClasses::white)
      {
        if (draw)
        {
          POINT_PX(ColorClasses::green, (unsigned int)(end.x), (unsigned int)(end.y));
        }
        whitePixels++;
        continue;
      }
      else
      {
        if (draw)
        {
          POINT_PX(ColorClasses::red, (unsigned int)(end.x), (unsigned int)(end.y));
        }
        //unsigned char y = theImage.get(end.x, end.y).y;
        //unsigned char cb = theImage.get(end.x, end.y).u;
        //unsigned char cr = theImage.get(end.x, end.y).v;
        //if(y > 100 && y > cb && y > cr)
          //whitePixels++;
        continue;
      }
    }
    else
    {
      return i;
    }
  }
  return scan.numberOfPixels;
}

inline bool exactLocalisation(const Image& theImage, const Vector2<int>& p)
{
  return (unsigned int)p.y <= theImage.cameraInfo.resolutionHeight;
}

#endif //__RobotDetectorFunctions_h_

