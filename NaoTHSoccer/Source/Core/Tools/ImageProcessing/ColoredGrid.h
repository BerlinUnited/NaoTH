#ifndef __ColoredGrid_H_
#define __ColoredGrid_H_

#include <Tools/Math/Vector2.h>
#include <Tools/ColorClasses.h>
#include <Tools/DataStructures/Printable.h>

#include "Grid/UniformGrid.h"

// TODO: remove it
using namespace naoth;

/**
 * Class ColoredGrid
 * describes a grid where every point has a specified color.
 * Access is possible using the point-index or using the color-index.
 *
 * Members:
 *  pointsCoordinates[] - Array containing the point coordinates (x,y)
 *  pointsColors[] - Array containing the colors , the points are labeled with
 *                   meaning: the Point i has the coordinates imagePoints[i] and the color pointsColors[i]
 *  numberOfGridPoints - number of Points in imagePoints[]
 *  
 *  // color-index structure:
 *  colorPointsIndex[][] - Array containing indexes of points in imagePoints[] for each color:
 *  colorPointsIndex[COLOR][] - Array containing the indexes of the points labeled with the color COLOR
 *                              e.g. p = colorPointsIndex[green][i] is the index of the i-th green point 
 *                                   (beginning with 0 :), then we the coordinates of the point p are 
 *                                   imagePoints[p] and if holds true pointsColors[p] == green
 *  numberOfColorPoints[] - Array containing the numbers of the points of each color
 *
 * CODE EXAMPLE:
 * (access points by color)

    // consider all green points
    for(int i = 0; i < numberOfColorPoints[green]; i++)
    {
      int p = colorPointsIndex[green][i];
      Vector2<int> x = pointsCoordinates[p];

      // .. do something with the point x ..
    }

 *                                   
 */
class ColoredGrid: public naoth::Printable
{
public:

  ColoredGrid(unsigned int imageWidth=320, unsigned int imageHeight=240, unsigned int width=60, unsigned int height=40)
    : uniformGrid(imageWidth, imageHeight, width, height)
  {
    pointsColors.resize(uniformGrid.maxNumberOfPoints);

    for(int i=0; i < ColorClasses::numOfColors; i++)
    {
      colorPointsIndex[i] = new unsigned int[width * height];
    }
    singlePointRate = 1.0 / (double) (uniformGrid.width * uniformGrid.height);

    reset();
  }//end constructor Grid

  virtual ~ColoredGrid()
  {
    for(int i=0; i < ColorClasses::numOfColors; i++)
    {
      delete[] colorPointsIndex[i];
    }
  }

  // constants
  //static const unsigned int NX = UniformGrid::MAXWIDTH;
  //static const unsigned int NY = UniformGrid::MAXHEIGHT;
  //static const unsigned int MAX_NUMBER_OF_POINTS = NX * NY;
  bool valid;
  double percentOfUnknownColors;
  double percentOfKnownColors;
  double meanBrightness;

  // the grid morphology
  UniformGrid uniformGrid;

  // colors of the grid points
  std::vector<ColorClasses::Color> pointsColors;

  double singlePointRate;
  
  //ColorClasses::Color classifiedImage[NX][NY];
  //ColorClasses classifiedImageIndex[NX][NY];


  // index structure for faster access to points of special color in the grid
  unsigned int* colorPointsIndex[ColorClasses::numOfColors];
  unsigned int numberOfColorPoints[ColorClasses::numOfColors];
  double colorPointsRate[ColorClasses::numOfColors];

  const Vector2<int>& getColorPoint(const int& pointIndex) const
  {
    return uniformGrid.getGridCoordinates(pointIndex);
    //return uniformGrid.gridCoordinates[pointIndex];
  }

  const Vector2<int>& getColorPoint(const ColorClasses::Color& color, const int& index) const
  {
    return uniformGrid.getGridCoordinates(colorPointsIndex[color][index]);
    //return uniformGrid.gridCoordinates[colorPointsIndex[color][index]];
  }

  const Vector2<int>& getImagePoint(const ColorClasses::Color& color, const int& index) const
  {
    return uniformGrid.getPoint(colorPointsIndex[color][index]);
    //return uniformGrid.pointsCoordinates[colorPointsIndex[color][index]];
  }

  const Vector2<int>& getImagePoint(const int& pointIndex) const
  {
    return uniformGrid.getPoint(pointIndex);
    //return uniformGrid.pointsCoordinates[pointIndex];
  }

  int getScaledImageIndex(const Vector2<int>& point) const
  {
    ASSERT(point.x >= 0);
    ASSERT(point.y >= 0);
    ASSERT(point.x < (int) uniformGrid.width);
    ASSERT(point.y < (int) uniformGrid.height);
    return uniformGrid.getScaledImageIndex(point.x, point.y);
  }

  int getScaledImageIndex(const int& x, const int& y) const
  {
    ASSERT(x >= 0);
    ASSERT(y >= 0);
    ASSERT(x < (int) uniformGrid.width);
    ASSERT(y < (int) uniformGrid.height);
    return uniformGrid.getScaledImageIndex(x, y);
  }

  const Vector2<int>& getImagePoint(const Vector2<int>& colorPoint) const
  {
    ASSERT(colorPoint.x >= 0);
    ASSERT(colorPoint.y >= 0);
    ASSERT(colorPoint.x < (int) uniformGrid.width);
    ASSERT(colorPoint.y < (int) uniformGrid.height);
    //return uniformGrid.pointsCoordinates[getScaledImageIndex(colorPoint)]; 
    return uniformGrid.getPoint(getScaledImageIndex(colorPoint));
  }

/*
  void addPoint(Vector2<int> point, ColorClasses::Color color)
  {
    if(grid.numberOfGridPoints < MAX_NUMBER_OF_POINTS)
    {
      // punkt in die liste eintragen (nach farben sortiert)
      pointsCoordinates[numberOfGridPoints] = point;
      pointsColors[numberOfGridPoints] = color;

      colorPointsIndex[color][numberOfColorPoints[color]] = numberOfGridPoints;
      numberOfColorPoints[color]++;
	     
      // punkt in die das reduzierte bild eintragen (nach koordinaten sortiert)
      int dx = point.x/(RobotInfo::getInstance().theCameraInfo.resolutionWidth/NX);
      int dy = point.y/(RobotInfo::getInstance().theCameraInfo.resolutionHeight/NY);
      classifiedImage[dx][dy] = color;
      classifiedImageIndex[dx][dy] = numberOfGridPoints;

	    numberOfGridPoints++;
    }//end if
  }//end addPoint
*/

  void setColor(const unsigned int& i, const ColorClasses::Color& color)
  {
    if(i < uniformGrid.numberOfGridPoints && numberOfColorPoints[color] < uniformGrid.maxNumberOfPoints)
    {
      pointsColors[i] = color;
      colorPointsRate[color] += singlePointRate;
      colorPointsIndex[color][numberOfColorPoints[color]] = i;
      numberOfColorPoints[color]++;
    }//end if
  }//end setColor


  void reset()
  {
    //numberOfGridPoints = 0;
    for(int i = 0; i < ColorClasses::numOfColors; i++)
    {
      numberOfColorPoints[i] = 0;
      colorPointsRate[i] = 0.0;
    }//end for
    percentOfUnknownColors = 0;
    percentOfKnownColors = 0;
    valid =  false;

    /*
    for(int i = 0; i < NX*NY; i++)
    {
      *(&(classifiedImageIndex[0][0])+i) = 0;
      *(&(classifiedImage[0][0])+i) = ColorClasses::none;
    }
    */
  }//end reset


  virtual void print(ostream& stream) const
  {
    stream << "Width = " << uniformGrid.width << endl
            << "Height = " << uniformGrid.height << endl
            << "Valid = " << (valid ? "true" : "false") << endl
            << "Mean Brightness = " << meanBrightness << endl
            << "Percent of known colors = " << percentOfKnownColors << endl
            << "Percent of unknown colors = " << percentOfUnknownColors << endl
            << "Rate of colors : " << endl;

    for(int i = 0; i < (int) ColorClasses::numOfColors; i++)
    {
      stream << "  " << ColorClasses::getColorName((ColorClasses::Color) i) << " = " << colorPointsRate[i] << endl;
    }


    // print something
  }//end print

};//end class ColoredGrid

#endif // __COLOREDGRID_H__

