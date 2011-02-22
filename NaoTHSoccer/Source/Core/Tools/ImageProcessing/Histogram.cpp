/* 
 * File:   histogram.cpp
 * Author: claas
 * 
 * Created on 9. Juli 2009, 13:40
 */

#include "Histogram.h"
#include "Core/Tools/Debug/DebugRequest.h"

Histogram::Histogram()
{
  std::stringstream dbgString;
  std::stringstream descString;
  for(int color = 0; color < ColorClasses::numOfColors; color++)
  {
    dbgString.str("");
    dbgString << "ImageProcessor:Histogram:x:" << ColorClasses::getColorName((ColorClasses::Color) color);
    descString.str("");
    descString << "draw X axis histogram of " << ColorClasses::getColorName((ColorClasses::Color) color) << " pixels";
    DEBUG_REQUEST_REGISTER(dbgString.str(), descString.str(), false);
    dbgString.str("");
    dbgString << "ImageProcessor:Histogram:y:" << ColorClasses::getColorName((ColorClasses::Color) color);
    descString.str("");
    descString << "draw Y axis histogram of " << ColorClasses::getColorName((ColorClasses::Color) color) << " pixels";
    DEBUG_REQUEST_REGISTER(dbgString.str(), descString.str(), false);

  }
//  init();
}

void Histogram::init()
{
  for(int color = 0; color < ColorClasses::numOfColors; color++)
  {
    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
    for(unsigned int x = 0; x < UniformGrid::HACK_MAX_WIDTH; x++)
    {
      yHistogram[color][x] = 0;
    }
    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
    for(unsigned int y = 0; y < UniformGrid::HACK_MAX_HEIGHT; y++)
    {
      xHistogram[color][y] = 0;
    }
  }
}//end init

void Histogram::execute()
{
  init();
}

void Histogram::showDebugInfos(const UniformGrid& grid, const CameraInfo& cameraInfo) const
{
  std::stringstream dbgString;
  bool drawXHist = false;
  bool drawYHist = false;

  for(int color = 0; color < ColorClasses::numOfColors; color++)
  {
    dbgString.str("");
    dbgString << "ImageProcessor:Histogram:x:" << ColorClasses::getColorName( (ColorClasses::Color) color);
    
    // TODO: put it in a macro?
    if(DebugRequest::getInstance().isActive(dbgString.str()))
    {
      drawXHist = true;
      Vector2<int> last(cameraInfo.resolutionWidth - (xHistogram[color][0] * 1), 0);
      for(unsigned int y = 1; y < (grid.height * 1); y += 1)
      {
        LINE_PX
        (
          (ColorClasses::Color) color,
          last.x,
          last.y,
          cameraInfo.resolutionWidth - (xHistogram[color][y] * 1),
          y
        );
        last.x = cameraInfo.resolutionWidth - (xHistogram[color][y] * 1);
        last.y = y;
      }
    }//end if DebugRequest

    dbgString.str("");
    dbgString << "ImageProcessor:Histogram:y:" << ColorClasses::getColorName((ColorClasses::Color) color);
    // TODO: put it in a macro?
    if(DebugRequest::getInstance().isActive(dbgString.str()))
    {
      drawYHist = true;
      Vector2<int> last(0, cameraInfo.resolutionHeight - (yHistogram[color][0] * 1) );
      for(unsigned int x = 1; x < (grid.width * 1); x += 1)
      {
        LINE_PX
        (
          (ColorClasses::Color) color,
          last.x,
          last.y,
          x,
          cameraInfo.resolutionHeight - (yHistogram[color][x] * 1)
        );
        last.x = x;
        last.y = cameraInfo.resolutionHeight - (yHistogram[color][x] * 1);
      }
    }//end if DebugRequest
  }

  if(drawYHist)
  {
      RECT_PX
      (
        ColorClasses::black,
        0,
        cameraInfo.resolutionHeight - (grid.height * 1),
        (grid.width * 1),
        cameraInfo.resolutionHeight
      );
  }

  if(drawXHist)
  {
      RECT_PX(
        ColorClasses::black,
        cameraInfo.resolutionWidth - (grid.width * 1),
        0,
        cameraInfo.resolutionWidth,
        grid.height * 1
      );
  }
}//end showDebugInfos

void Histogram::increaseValue(int x, int y, ColorClasses::Color color)
{
    xHistogram[color][y]++;
    yHistogram[color][x]++;
}//end increaseValue

void Histogram::increaseValue(const UniformGrid& grid, int pixelIndex, ColorClasses::Color color)
{
    Vector2<int> pixel = grid.getGridCoordinates(pixelIndex);
//      if(pixel.y < UniformGrid::MAXHEIGHT)
      xHistogram[color][pixel.y]++;
////      if(pixel.x < UniformGrid::MAXWIDTH)
      yHistogram[color][pixel.x]++;
}//end increaseValue

void Histogram::createFromColoredGrid(const ColoredGrid& coloredGrid)
{
  for(int color = 0; color < ColorClasses::numOfColors; color++)
  {
    for (unsigned int pixelIndex = 0; pixelIndex < coloredGrid.numberOfColorPoints[color]; pixelIndex++)
    {
      increaseValue(coloredGrid.uniformGrid, coloredGrid.colorPointsIndex[color][pixelIndex], (ColorClasses::Color) color);
    }
  }
}//end createFromColoredGrid

void Histogram::print(ostream& stream) const
{
  stream << "Histogram";
}//end print




