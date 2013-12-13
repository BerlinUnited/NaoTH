#ifndef __UniformGrid_H_
#define __UniformGrid_H_

#include <vector>
#include <Tools/Math/Vector2.h>

#include "Grid.h"

class UniformGrid: public Grid
{
public:
  UniformGrid(unsigned int imageWidth, unsigned int imageHeight, unsigned int gridWidth, unsigned int gridHeight)
    : 
    Grid(gridWidth*gridHeight),
    width(gridWidth),
    height(gridHeight),
    scaledImageIndex(gridWidth * gridHeight),
    gridCoordinates(gridWidth * gridHeight)
  {
    //scaledImageIndex = new int[gridWidth * gridHeight];
    //gridCoordinates = new Vector2<int>[gridWidth * gridHeight];

    //CameraInfo& cameraInfo = Platform::getInstance().theCameraInfo;
    createUniformGrid(imageWidth, imageHeight, width, height);
  }

  virtual ~UniformGrid(){}

  // dimensions of the grid
  const unsigned int width;
  const unsigned int height;


  inline void addPoint(const Vector2<int>& point, unsigned int gridX, unsigned int gridY)
  {
    int idx = Grid::addPoint(point); // ensures 0 <= idx < width*height

    //getScaledImageIndex(gridX,gridY) = idx;

    // TODO: find a better solution for it
    ASSERT(gridX < width && gridY < height);
    scaledImageIndex[gridY * width + gridX] = idx;

    gridCoordinates[idx].x = gridX;
    gridCoordinates[idx].y = gridY;
  }//end addPoint


  int getScaledImageIndex(unsigned int x, unsigned int y) const
  {
    ASSERT(x < width && y < height);
    return scaledImageIndex[y * width + x];
  }

  inline const Vector2<int>& getGridCoordinates(unsigned int idx) const
  {
    ASSERT(idx < Grid::maxNumberOfPoints);
    return gridCoordinates[idx];
  }

  void createUniformGrid(const int imageWidth, const int imageHeight, const int gridWidth, const int gridHeight)
  {
    int deltaX = imageWidth / gridWidth;
    int deltaY = imageHeight / gridHeight;

    int offsetX = deltaX / 2;
    int offsetY = deltaY / 2;

    int restX = imageWidth % gridWidth;
    int restY = imageHeight % gridHeight;

    
    int gy = offsetY;

    for(int y = 0; y < gridHeight; y++)
    {
      int gx = offsetX;

      for(int x = 0; x < gridWidth; x++)
      {
        addPoint(Vector2<int>(gx, gy), x, y);
        gx += deltaX + ((restX > x) ? 1 : 0);
      }//end for
      gy += deltaY + ((restY > y) ? 1 : 0);
    }//end for
  }//end createUniformGrid

private:
  
  std::vector<int> scaledImageIndex;
  std::vector<Vector2<int> > gridCoordinates;

};//end class UniformGrid

#endif // __UniformGrid_H__

