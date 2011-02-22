/**
* @file GridProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class GridProvider
*/

#include "GridProvider.h"

#include "Core/Tools/Debug/DebugRequest.h"

GridProvider::GridProvider()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:show_grid", "show the image processing grid", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:show_classified_image", "draw the image represented by uniformGrid", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorTable:create_color_table","...", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorTable:create_color_table:extended","...", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:Histogram:enable_debug", "Enables the debug output for the histogram", false);
}


void GridProvider::execute()
{
  // fill the grid
  calculateColoredGrid();


  // make some debug

  DEBUG_REQUEST("ImageProcessor:ColorTable:create_color_table",
    createColorTable();
  );

  DEBUG_REQUEST("ImageProcessor:Histogram:enable_debug",
    getHistogram().showDebugInfos(getColoredGrid().uniformGrid, getImage().cameraInfo);
  );

  DEBUG_REQUEST("ImageProcessor:show_classified_image",
    for(unsigned int x=0; x<getColoredGrid().uniformGrid.width; x++)
    {
      for(unsigned int y=0; y<getColoredGrid().uniformGrid.height; y++)
      {
        POINT_PX(getColoredGrid().pointsColors[getColoredGrid().getScaledImageIndex(x,y)], (unsigned int)x, (unsigned int)y);
      }
    }
  );//end DEBUG
}//end execute

// hier wird das Gitter eingefaerbt (die Faerbung erfolgt fuer beliebige Gitter glech,
// daher wird es nicht im GridCreator gemacht)
void GridProvider::calculateColoredGrid()//const Grid& grid)//, ColoredGrid& coloredGrid, Histogram& histogram)
{
  STOPWATCH_START("Histogram+ColoredGrid");
  getColoredGrid().reset();
  getHistogram().init();

//  unsigned int unknownColorCount = 0;
//  unsigned int knownColorCount = 0;
  double grey = 0;

  for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
  {
    //Vector2<int> point = getColoredGrid().uniformGrid.pointsCoordinates[i];
    Vector2<int> point = getColoredGrid().uniformGrid.getPoint(i);

    Pixel pixel = getImage().get(point.x,point.y);

    grey += pixel.y;

    ColorClasses::Color currentPixelColor = getColorTable64().getColorClass(pixel);
    if(currentPixelColor == ColorClasses::none)
    {
//      unknownColorCount++;
      getColoredGrid().percentOfUnknownColors += getColoredGrid().singlePointRate;
    }
    if
    (
      currentPixelColor == ColorClasses::white ||
      currentPixelColor == ColorClasses::green ||
      currentPixelColor == ColorClasses::orange ||
      currentPixelColor == ColorClasses::skyblue ||
      currentPixelColor == ColorClasses::yellow
    )
    {
//      knownColorCount++;
      getColoredGrid().percentOfKnownColors += getColoredGrid().singlePointRate;
    }
    getColoredGrid().setColor(i, currentPixelColor);
    getHistogram().increaseValue(getColoredGrid().uniformGrid, i, currentPixelColor);
  }//end for
//  getColoredGrid().percentOfUnknownColors = ((double) unknownColorCount) / ( ((double) getColoredGrid().uniformGrid.width ) * ( ( double) getColoredGrid().uniformGrid.height) );
//  getColoredGrid().percentOfKnownColors = ((double) knownColorCount) / ( ((double) getColoredGrid().uniformGrid.width ) * ( ( double) getColoredGrid().uniformGrid.height) );

  getColoredGrid().valid = false;
  if(getColoredGrid().percentOfUnknownColors < 85 && getColoredGrid().percentOfKnownColors > 20)
  {
    getColoredGrid().valid = true;
  }

  getColoredGrid().meanBrightness = grey / (getColoredGrid().uniformGrid.height * getColoredGrid().uniformGrid.width);

  STOPWATCH_STOP("Histogram+ColoredGrid");
  
  DEBUG_REQUEST("ImageProcessor:show_grid",
    for(unsigned int i = 0; i < getColoredGrid().uniformGrid.numberOfGridPoints; i++)
    {
      //Vector2<int> point = getColoredGrid().uniformGrid.pointsCoordinates[i];
      Vector2<int> point = getColoredGrid().uniformGrid.getPoint(i);
      POINT_PX(getColoredGrid().pointsColors[i], point.x, point.y);
    }//end for
  );
}//end calculateColoredGrid

void GridProvider::createColorTable() const
{
  bool create_extended_color_table = false;
  DEBUG_REQUEST("ImageProcessor:ColorTable:create_color_table:extended",
    create_extended_color_table = true;
  );

  for(int Y = 0; Y < 64; Y++)
  {
    for(int Cb = 0; Cb < 64; Cb++)
    {
      for(int Cr = 0; Cr < 64; Cr++)
      {
        ColorClasses::Color color = ColorClasses::none;
        unsigned char H = 0;
        unsigned char S = 0;
        unsigned char I = 0;
        ColorModelConversions::fromYCbCrToHSI(Y << 2,  Cb << 2, Cr << 2, H, S, I);

        if(I <= 20) //black
        {
          if(create_extended_color_table)
          {
            color = ColorClasses::black;
          }
        }
        else if(S <= 10)//40)//60) //white or gray or black
        {
          if(I >= 150) //white
          {
            color = ColorClasses::white;
          }
          else if(I <= 60) //black
          {
            if(create_extended_color_table)
            {
              color = ColorClasses::black;
            }
          }
          else
          {
            if(create_extended_color_table)
            {
              color = ColorClasses::gray;
            }
          }
        }
        else
        {
          if(S > 40 && (H < 25 || H >= 200))
          {
            if(I <= 80)
            {
              if(create_extended_color_table)
              {
                color = ColorClasses::blue;
              }
            }
            else if(I <= 240)
            {
              color = ColorClasses::skyblue;
            }
          }
          if(H >= 35 && H < 90)
          {
            color = ColorClasses::green;
          }
          if(S > 40 && H >= 90 && H < 140)
          {
            if(I >= 150)
            {
              color = ColorClasses::yellow;
            }
          }
          if(S > 40 && H >= 130 && H < 135)
          {
            //color = ColorClasses::yellowOrange;
            color = ColorClasses::yellow;
          }
          if(S > 40 && H >= 135 && H < 150)
          {
//              color = ColorClasses::orange;
            color = ColorClasses::yellowOrange;
          }
          if(S > 40 && H >= 150 && H < 200)
          {
            if(I <= 60) //black
            {
              if(create_extended_color_table)
              {
                color = ColorClasses::black;
              }
            }
            else if(I <= 120) //red
            {
              if(create_extended_color_table)
              {
                color = ColorClasses::red;
              }
            }
            else
            {
              color = ColorClasses::orange;
            }
          }
        }
        // TODO: set this!
        //getColorTable64().setColorClass(color, Y << 2, Cb << 2, Cr << 2);
      }//end for
    }//end for
  }//end for
}//end create_color_table
