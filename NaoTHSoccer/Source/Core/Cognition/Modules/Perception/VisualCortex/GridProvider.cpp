/**
* @file GridProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class GridProvider
*/

#include "GridProvider.h"

// Debug
#include <Tools/Debug/Stopwatch.h>


GridProvider::GridProvider()
:
  cameraID(CameraInfo::Bottom)
{
  DEBUG_REQUEST_REGISTER("Vision:show_grid", "show the image processing grid", false);
  DEBUG_REQUEST_REGISTER("Vision:show_classified_image", "draw the image represented by uniformGrid", false);
  DEBUG_REQUEST_REGISTER("Vision:ColorClassesHistograms:enable_debug", "Enables the debug output for the histogram", false);
}

void GridProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(id);

  // fill the grid
  STOPWATCH_START("ColoredGrid");
  calculateColoredGrid();
  STOPWATCH_STOP("ColoredGrid");

  // make some debug
  DEBUG_REQUEST("Vision:show_grid",
    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++) {
      const Vector2i& point = getColoredGrid().uniformGrid.getPoint(i);
      POINT_PX(getColoredGrid().pointsColors[i], point.x, point.y);
    }
  );

  DEBUG_REQUEST("Vision:ColorClassesHistograms:enable_debug",
    getColorClassesHistograms().showDebugInfos(getColoredGrid().uniformGrid, getImage().cameraInfo);
  );

  DEBUG_REQUEST("Vision:show_classified_image",
    for(int x=0; x<(int)getColoredGrid().uniformGrid.width; x++) {
      for(int y=0; y<(int)getColoredGrid().uniformGrid.height; y++) {
        POINT_PX(getColoredGrid().pointsColors[getColoredGrid().getScaledImageIndex(x,y)], x, y);
      }
    }
  );
}//end execute

// hier wird das Gitter eingefaerbt (die Faerbung erfolgt fuer beliebige Gitter gleich,
// daher wird es nicht im GridCreator gemacht)
void GridProvider::calculateColoredGrid()//const Grid& grid)//, ColoredGrid& coloredGrid, Histogram& histogram)
{
  getColoredGrid().reset();
  getColorClassesHistograms().init();

  Pixel pixel;
  
  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    const Vector2i& point = getColoredGrid().uniformGrid.getPoint(i);

    getImage().get(point.x, point.y, pixel);

    // classify the color
    ColorClasses::Color currentPixelColor = getColorClassificationModel().getColorClass(pixel);

    const Vector2i& gridPoint = getColoredGrid().uniformGrid.getGridCoordinates(i);
    getColorClassesHistograms().increaseValue(gridPoint, currentPixelColor);
    
    // remember the color in the grid
    getColoredGrid().setColor(i, currentPixelColor);
  }//end for

  getColoredGrid().validate();
}//end calculateColoredGrid

