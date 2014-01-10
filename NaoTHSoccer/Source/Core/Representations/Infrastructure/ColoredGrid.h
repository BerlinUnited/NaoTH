#ifndef _ColoredGrid_H_
#define _ColoredGrid_H_

#include <Tools/Math/Vector2.h>
#include <Tools/ColorClasses.h>
#include <Tools/DataStructures/Printable.h>

#include <Representations/Infrastructure/CameraInfo.h>
#include <Tools/DataStructures/UniformGrid.h>


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
      Vector2i x = pointsCoordinates[p];

      // .. do something with the point x ..
    }

 *                                   
 */
class ColoredGrid: public naoth::Printable
{
public:

  ColoredGrid(unsigned int imageWidth=naoth::IMAGE_WIDTH, unsigned int imageHeight=naoth::IMAGE_HEIGHT, unsigned int width=60, unsigned int height=40)
    : 
    uniformGrid(imageWidth, imageHeight, width, height),
    pointsColors(width*height)
  {
    for(int i=0; i < ColorClasses::numOfColors; i++) {
      colorPointsIndex[i] = new unsigned int[width * height];
    }
    singlePointRate = 1.0 / (double) (uniformGrid.width * uniformGrid.height);

    reset();
  }

  virtual ~ColoredGrid()
  {
    for(int i=0; i < ColorClasses::numOfColors; i++) {
      delete[] colorPointsIndex[i];
    }
  }

  bool valid;
  double percentOfUnknownColors;
  double percentOfKnownColors;

  // the grid morphology
  UniformGrid uniformGrid;

  // colors of the grid points
  std::vector<ColorClasses::Color> pointsColors;

  double singlePointRate;
  
//private:
  // index structure for faster access to points of special color in the grid
  unsigned int* colorPointsIndex[ColorClasses::numOfColors];
  unsigned int numberOfColorPoints[ColorClasses::numOfColors];
  double colorPointsRate[ColorClasses::numOfColors];

public:
  inline const Vector2i& getColorPoint(const int& pointIndex) const {
    return uniformGrid.getGridCoordinates(pointIndex);
  }

  inline const Vector2i& getColorPoint(const ColorClasses::Color& color, const int& index) const {
    return uniformGrid.getGridCoordinates(colorPointsIndex[color][index]);
  }

  inline const Vector2i& getImagePoint(const ColorClasses::Color& color, const int& index) const {
    return uniformGrid.getPoint(colorPointsIndex[color][index]);
  }

  inline const Vector2i& getImagePoint(const int& pointIndex) const {
    return uniformGrid.getPoint(pointIndex);
  }

  inline int getScaledImageIndex(const Vector2i& point) const
  {
    ASSERT(point.x >= 0 &&
           point.y >= 0 &&
           point.x < (int) uniformGrid.width &&
           point.y < (int) uniformGrid.height);
    return uniformGrid.getScaledImageIndex(point.x, point.y);
  }

  inline int getScaledImageIndex(const int& x, const int& y) const
  {
    ASSERT(x >= 0 &&
           y >= 0 &&
           x < (int) uniformGrid.width &&
           y < (int) uniformGrid.height);
    return uniformGrid.getScaledImageIndex(x, y);
  }

  inline const Vector2i& getImagePoint(const Vector2i& colorPoint) const
  {
    ASSERT(colorPoint.x >= 0 &&
           colorPoint.y >= 0 &&
           colorPoint.x < (int) uniformGrid.width &&
           colorPoint.y < (int) uniformGrid.height);
    return uniformGrid.getPoint(getScaledImageIndex(colorPoint));
  }

  inline void setColor(const unsigned int i, const ColorClasses::Color& color)
  {
    ASSERT(i < uniformGrid.width*uniformGrid.height && numberOfColorPoints[color] < uniformGrid.width*uniformGrid.height);

    pointsColors[i] = color;
    colorPointsRate[color] += singlePointRate;
    colorPointsIndex[color][numberOfColorPoints[color]] = i;
    numberOfColorPoints[color]++;
  }//end setColor


  // check whether the grid looks valid...
  void validate()
  {
    for(int color = ColorClasses::orange; color < ColorClasses::numOfColors; color++) {
      percentOfKnownColors += colorPointsRate[color];
    }
        
    percentOfUnknownColors = colorPointsRate[ColorClasses::none];
     
    // check if enough known colors were detected
    //TODO: the criteria is a kind of strange..
    if(percentOfUnknownColors < 85 && percentOfKnownColors > 20) {
      valid = true;
    } else {
      valid = false;
    }
  }//end validate


  void reset()
  {
    for(int i = 0; i < ColorClasses::numOfColors; i++) {
      numberOfColorPoints[i] = 0;
      colorPointsRate[i] = 0.0;
    }

    // statistics
    percentOfUnknownColors = 0;
    percentOfKnownColors = 0;

    valid = false;
  }//end reset


  virtual void print(std::ostream& stream) const
  {
    stream << "Width = " << uniformGrid.width << std::endl
            << "Height = " << uniformGrid.height << std::endl
            << "Valid = " << (valid ? "true" : "false") << std::endl
            << "Percent of known colors = " << percentOfKnownColors << std::endl
            << "Percent of unknown colors = " << percentOfUnknownColors << std::endl
            << "Rate of colors : " << std::endl;

    for(int i = 0; i < (int) ColorClasses::numOfColors; i++) {
      stream << "  " << ColorClasses::getColorName((ColorClasses::Color) i) << " = " << colorPointsRate[i] << std::endl;
    }
  }//end print

};//end class ColoredGrid

class ColoredGridTop : public ColoredGrid
{
public:
  virtual ~ColoredGridTop() {}
};


#endif // _COLOREDGRID_H__

