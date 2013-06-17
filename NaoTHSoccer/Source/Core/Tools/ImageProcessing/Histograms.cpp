/*
 * File:   histograms.cpp
 * Author: claas
 *
 * Created on 9. Juli 2009, 13:40
 */

#include "Histograms.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"

using namespace naoth;
using namespace std;

Histograms::Histograms()
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

  //DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:colorChannelHistogramCr", "draw X axis rate histogram of the V (Cr) color channel from the field", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:plotY", "plot Y channel histogram bottom image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:plotU", "plot U channel histogram bottom image", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:plotV", "plot V channel histogram bottom image", false);
 
  init();
}

void Histograms::init()
{
  colorChannelIsUptodate = false;

  for(int c = 0; c < ColorClasses::numOfColors; c++)
  {
    xHistogram[c].clear();
    yHistogram[c].clear();
  }
}//end init

void Histograms::execute()
{
  init();
}

void Histograms::showDebugInfos(const UniformGrid& grid, const CameraInfo& cameraInfo) const
{
  std::stringstream dbgString;
  bool drawXHist = false;
  bool drawYHist = false;
  //bool drawChannelHist = false;

  DEBUG_REQUEST("ImageProcessor:Histogram:plotY", 
    histogramY.plot("Histograms:Y");
  );
  DEBUG_REQUEST("ImageProcessor:Histogram:plotU", 
    histogramU.plot("Histograms:U");
  );
  DEBUG_REQUEST("ImageProcessor:Histogram:plotV", 
    histogramV.plot("Histograms:V");
  );

  for(int color = 0; color < ColorClasses::numOfColors; color++)
  {
    dbgString.str("");
    dbgString << "ImageProcessor:Histogram:x:" << ColorClasses::getColorName( (ColorClasses::Color) color);
    DEBUG_REQUEST_GENERIC(dbgString.str(),
      drawXHist = true;
    Vector2<int> last(cameraInfo.resolutionWidth - (xHistogram[color].rawData[0] * 1), 0);
      for(unsigned int y = 1; y < (grid.height * 1); y += 1)
      {
        LINE_PX
        (
          (ColorClasses::Color) color,
          last.x,
          last.y,
          cameraInfo.resolutionWidth - (xHistogram[color].rawData[y] * 1),
          y
        );
        last.x = cameraInfo.resolutionWidth - (xHistogram[color].rawData[y] * 1);
        last.y = y;
      }
    );

    dbgString.str("");
    dbgString << "ImageProcessor:Histogram:y:" << ColorClasses::getColorName((ColorClasses::Color) color);
    DEBUG_REQUEST_GENERIC(dbgString.str(),
      drawYHist = true;
    Vector2<int> last(0, cameraInfo.resolutionHeight - (yHistogram[color].rawData[0] * 1) );
      for(unsigned int x = 1; x < (grid.width * 1); x += 1)
      {
        LINE_PX
        (
          (ColorClasses::Color) color,
          last.x,
          last.y,
          x,
          cameraInfo.resolutionHeight - (yHistogram[color].rawData[x] * 1)
        );
        last.x = x;
        last.y = cameraInfo.resolutionHeight - (yHistogram[color].rawData[x] * 1);
      }
    );
  }

  //DEBUG_REQUEST("ImageProcessor:Histogram:colorChannelHistogramCr",
  //  drawChannelHist = true;
  //  Vector2<int> last(0, Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramCr[0], 0, (int)cameraInfo.resolutionHeight));
  //  for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
  //  {
  //    LINE_PX
  //    (
  //      ColorClasses::red,
  //      last.x,
  //      last.y,
  //      x,
  //      Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramCr[x], 0, (int)cameraInfo.resolutionHeight)
  //    );
  //    last.x = x;
  //    last.y = Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramCr[x], 0, (int)cameraInfo.resolutionHeight);
  //  }
  //);

  //if(drawChannelHist)
  //{
  //  RECT_PX
  //  (
  //    ColorClasses::black,
  //    0,
  //    cameraInfo.resolutionHeight,
  //    COLOR_CHANNEL_VALUE_COUNT,
  //    0
  //  );
  //}

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

}//end showDebugInfos


inline void Histograms::createFromColoredGrid(const ColoredGrid& coloredGrid)
{
  for(int color = 0; color < ColorClasses::numOfColors; color++)
  {
    for (unsigned int pixelIndex = 0; pixelIndex < coloredGrid.numberOfColorPoints[color]; pixelIndex++)
    {
      const Vector2<int>& gridPoint = coloredGrid.uniformGrid.getGridCoordinates(pixelIndex);
      increaseValue(gridPoint, (ColorClasses::Color) color);
    }
  }
}//end createFromColoredGrid

void Histograms::print(ostream& stream) const
{
  stream << "Histogram";
}//end print

