/*
 * File:   Histograms.cpp
 * @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
 *
 * Created on 9. Juli 2009, 13:40
 */

#include "Histograms.h"

using namespace naoth;

ColorClassesHistograms::ColorClassesHistograms()
{ 
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

void ColorClassesHistograms::print(std::ostream& stream) const
{
  stream << "ColorClassesHistograms";
}//end print


ColorChannelHistograms::ColorChannelHistograms()
{}

void OverTimeHistogram::print(std::ostream& stream) const
{
  stream << "OverTimeHistogram" << std::endl;
  stream << "----------------------" << std::endl << std::endl;
  stream << "meanEnv = (" << meanEnv.y << ", " << meanEnv.u << ", " << meanEnv.v << ")" << std::endl;
  stream << "meanImg = (" << meanImg.y << ", " << meanImg.u << ", " << meanImg.v << ")" << std::endl;
  stream << "minEnv = (" << minEnv.y << ", " << minEnv.u << ", " << maxEnv.v << ")" << std::endl;
  stream << "maxEnv = (" << maxEnv.y << ", " << maxEnv.u << ", " << maxEnv.v << ")" << std::endl;
  stream << "spanWidthEnv = (" << spanWidthEnv.y << ", " << spanWidthEnv.u << ", " << spanWidthEnv.v << ")" << std::endl;
}//end print


