/*
 * File:   histogram.cpp
 * Author: claas
 *
 * Created on 9. Juli 2009, 13:40
 */

#include "Histogram.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"


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

  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:0_Y_color_channel", "draw X axis histogram of the Y color channel", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:1_Cb_color_channel", "draw X axis histogram of the U (Cb) color channel", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:2_Cr_color_channel", "draw X axis histogram of the V (Cr) color channel", false);

  init();
}

void Histogram::init()
{
  colorChannelIsUptodate = false;

  memset(&xHistogram, 0, sizeof(xHistogram));
  memset(&yHistogram, 0, sizeof(xHistogram));
  memset(&colorChannelHistogramField, 0, sizeof(colorChannelHistogramField));
  //memset(&colorChannelHistogramLine, 0, sizeof(colorChannelHistogramLine));
  //memset(&colorChannelHistogramGoal, 0, sizeof(colorChannelHistogramGoal));
  //memset(&colorChannelHistogramBall, 0, sizeof(colorChannelHistogramBall));

  meanFieldY = 0.0;
  meanFieldCountY = 1.0;
  //weightedHistCb = {0.0};
  memset(&weightedHistY, 0, sizeof(weightedHistY));
  memset(&weightedHistCb, 0, sizeof(weightedHistCb));

//  for(int color = 0; color < ColorClasses::numOfColors; color++)
//  {
//    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
//    for(unsigned int x = 0; x < UniformGrid::HACK_MAX_WIDTH; x++)
//    {
//      yHistogram[color][x] = 0;
//    }
//    // FIXME: remove HACK_MAX_HEIGHT & HACK_MAX_WIDTH
//    for(unsigned int y = 0; y < UniformGrid::HACK_MAX_HEIGHT; y++)
//    {
//      xHistogram[color][y] = 0;
//    }
//  }

//  for(int value = 0; value < COLOR_CHANNEL_VALUE_COUNT; value++)
//  {
//    for(int channel = 0; channel < 3; channel++)
//    {
//      colorChannelHistogram[channel][value] = 0;
//    }
//  }


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
  bool drawChannelHist = false;

  for(int color = 0; color < ColorClasses::numOfColors; color++)
  {
    dbgString.str("");
    dbgString << "ImageProcessor:Histogram:x:" << ColorClasses::getColorName( (ColorClasses::Color) color);
    DEBUG_REQUEST_GENERIC(dbgString.str(),
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
    );

    dbgString.str("");
    dbgString << "ImageProcessor:Histogram:y:" << ColorClasses::getColorName((ColorClasses::Color) color);
    DEBUG_REQUEST_GENERIC(dbgString.str(),
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
    );
  }

  DEBUG_REQUEST("ImageProcessor:Histogram:0_Y_color_channel",
    drawChannelHist = true;
    Vector2<int> last(0, Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramField[0][0], 0, (int)cameraInfo.resolutionHeight) );
    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
    {
      LINE_PX
      (
        ColorClasses::white,
        last.x,
        last.y,
        x,
        Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramField[0][x], 0, (int)cameraInfo.resolutionHeight)
      );
      last.x = x;
      last.y = Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramField[0][x], 0, (int)cameraInfo.resolutionHeight);
    }
  );

  DEBUG_REQUEST("ImageProcessor:Histogram:1_Cb_color_channel",
    drawChannelHist = true;
    Vector2<int> last(0, Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramField[1][0], 0, (int)cameraInfo.resolutionHeight));
    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
    {
      LINE_PX
      (
        ColorClasses::blue,
        last.x,
        last.y,
        x,
        Math::clamp((int)cameraInfo.resolutionHeight - (int)  colorChannelHistogramField[1][x], 0, (int)cameraInfo.resolutionHeight)
      );
      last.x = x;
      last.y = Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramField[1][x], 0, (int)cameraInfo.resolutionHeight);
    }
  );

  DEBUG_REQUEST("ImageProcessor:Histogram:2_Cr_color_channel",
    drawChannelHist = true;
    Vector2<int> last(0, Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramField[2][0], 0, (int)cameraInfo.resolutionHeight));
    for(unsigned int x = 1; x < COLOR_CHANNEL_VALUE_COUNT; x ++)
    {
      LINE_PX
      (
        ColorClasses::red,
        last.x,
        last.y,
        x,
        Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramField[2][x], 0, (int)cameraInfo.resolutionHeight)
      );
      last.x = x;
      last.y = Math::clamp((int)cameraInfo.resolutionHeight - (int) colorChannelHistogramField[2][x], 0, (int)cameraInfo.resolutionHeight);
    }
  );

  if(drawChannelHist)
  {
    RECT_PX
    (
      ColorClasses::black,
      0,
      cameraInfo.resolutionHeight,
      COLOR_CHANNEL_VALUE_COUNT,
      0
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


inline void Histogram::createFromColoredGrid(const ColoredGrid& coloredGrid)
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

