/*
 * File:   Histograms.cpp
 * @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
 *
 * Created on 9. Juli 2009, 13:40
 */

#include "Histograms.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"

using namespace naoth;
using namespace std;

ColorClassesHistograms::ColorClassesHistograms()
{
  std::stringstream dbgString;
  std::stringstream descString;
  //for(int color = 0; color < ColorClasses::numOfColors; color++)
  //{
  //  dbgString.str("");
  //  dbgString << "Vision:ColorClassesHistograms:x:" << ColorClasses::getColorName((ColorClasses::Color) color);
  //  descString.str("");
  //  descString << "draw X axis histogram of " << ColorClasses::getColorName((ColorClasses::Color) color) << " pixels";
  //  DEBUG_REQUEST_REGISTER(dbgString.str(), descString.str(), false);
  //  dbgString.str("");
  //  dbgString << "Vision:ColorClassesHistograms:y:" << ColorClasses::getColorName((ColorClasses::Color) color);
  //  descString.str("");
  //  descString << "draw Y axis histogram of " << ColorClasses::getColorName((ColorClasses::Color) color) << " pixels";
  //  DEBUG_REQUEST_REGISTER(dbgString.str(), descString.str(), false);
  //}
 
  init();
}

void ColorClassesHistograms::init()
{
  for(int c = 0; c < ColorClasses::numOfColors; c++)
  {
    xHistogram[c].clear();
    yHistogram[c].clear();
  }
}//end init


void ColorClassesHistograms::showDebugInfos(const UniformGrid& grid, const CameraInfo& cameraInfo) const
{
  std::stringstream dbgString;
  bool drawXHist = false;
  bool drawYHist = false;

  //for(int color = 0; color < ColorClasses::numOfColors; color++)
  //{
  //  dbgString.str("");
  //  dbgString << "Vision:ColorClassesHistograms:x:" << ColorClasses::getColorName( (ColorClasses::Color) color);
  //  DEBUG_REQUEST_GENERIC(dbgString.str(),
  //    drawXHist = true;
  //  Vector2i last(cameraInfo.resolutionWidth - (xHistogram[color].rawData[0] * 1), 0);
  //    for(unsigned int y = 1; y < (grid.height * 1); y += 1)
  //    {
  //      LINE_PX
  //      (
  //        (ColorClasses::Color) color,
  //        last.x,
  //        last.y,
  //        cameraInfo.resolutionWidth - (xHistogram[color].rawData[y] * 1),
  //        y
  //      );
  //      last.x = cameraInfo.resolutionWidth - (xHistogram[color].rawData[y] * 1);
  //      last.y = y;
  //    }
  //  );

  //  dbgString.str("");
  //  dbgString << "Vision:ColorClassesHistograms:y:" << ColorClasses::getColorName((ColorClasses::Color) color);
  //  DEBUG_REQUEST_GENERIC(dbgString.str(),
  //    drawYHist = true;
  //  Vector2i last(0, cameraInfo.resolutionHeight - (yHistogram[color].rawData[0] * 1) );
  //    for(unsigned int x = 1; x < (grid.width * 1); x += 1)
  //    {
  //      LINE_PX
  //      (
  //        (ColorClasses::Color) color,
  //        last.x,
  //        last.y,
  //        x,
  //        cameraInfo.resolutionHeight - (yHistogram[color].rawData[x] * 1)
  //      );
  //      last.x = x;
  //      last.y = cameraInfo.resolutionHeight - (yHistogram[color].rawData[x] * 1);
  //    }
  //  );
  //}

  //DEBUG_REQUEST("Vision:Histograms:colorChannelHistogramCr",
  //  drawChannelHist = true;
  //  Vector2i last(0, Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramCr[0], 0, (int)cameraInfo.resolutionHeight));
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
    //RECT_PX(
    //  ColorClasses::black,
    //  cameraInfo.resolutionWidth - (grid.width * 1),
    //  0,
    //  cameraInfo.resolutionWidth,
    //  grid.height * 1
    //);
  }

  if(drawYHist)
  {
    //RECT_PX
    //(
    //  ColorClasses::black,
    //  0,
    //  cameraInfo.resolutionHeight - (grid.height * 1),
    //  (grid.width * 1),
    //  cameraInfo.resolutionHeight
    //);
  }

}//end showDebugInfos


inline void ColorClassesHistograms::createFromColoredGrid(const ColoredGrid& coloredGrid)
{
  for(int color = 0; color < ColorClasses::numOfColors; color++)
  {
    for (unsigned int pixelIndex = 0; pixelIndex < coloredGrid.numberOfColorPoints[color]; pixelIndex++)
    {
      const Vector2i& gridPoint = coloredGrid.uniformGrid.getGridCoordinates(pixelIndex);
      increaseValue(gridPoint, (ColorClasses::Color) color);
    }
  }
}//end createFromColoredGrid

void ColorClassesHistograms::print(ostream& stream) const
{
  stream << "ColorClassesHistograms";
}//end print


ColorChannelHistograms::ColorChannelHistograms()
{
  //DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:plotY", "plot Y channel histogram bottom image", false);
  //DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:plotU", "plot U channel histogram bottom image", false);
  //DEBUG_REQUEST_REGISTER("Vision:ColorChannelHistograms:plotV", "plot V channel histogram bottom image", false);
}

void ColorChannelHistograms::showDebugInfos() const
{
  //DEBUG_REQUEST("Vision:ColorChannelHistograms:plotY", 
  //  histogramY.plot("Histograms:Y");
  //);
  //DEBUG_REQUEST("Vision:ColorChannelHistograms:plotU", 
  //  histogramU.plot("Histograms:U");
  //);
  //DEBUG_REQUEST("Vision:ColorChannelHistograms:plotV", 
  //  histogramV.plot("Histograms:V");
  //);
}

void OverTimeHistogram::print(ostream& stream) const
{
  stream << "OverTimeHistogram" << std::endl;
  stream << "----------------------" << std::endl << std::endl;
  stream << "meanEnv = (" << meanEnv.y << ", " << meanEnv.u << ", " << meanEnv.v << ")" << std::endl;
  stream << "meanImg = (" << meanImg.y << ", " << meanImg.u << ", " << meanImg.v << ")" << std::endl;
  stream << "minEnv = (" << minEnv.y << ", " << minEnv.u << ", " << maxEnv.v << ")" << std::endl;
  stream << "maxEnv = (" << maxEnv.y << ", " << maxEnv.u << ", " << maxEnv.v << ")" << std::endl;
  stream << "spanWidthEnv = (" << spanWidthEnv.y << ", " << spanWidthEnv.u << ", " << spanWidthEnv.v << ")" << std::endl;
}//end print


