#ifndef __UniformGrid_H_
#define __UniformGrid_H_

#include "Tools/Math/Vector2.h"
#include "PlatformInterface/Platform.h"

#include "Grid.h"

#include <Representations/Infrastructure/CameraInfo.h>

using namespace naoth;

class UniformGrid: public Grid
{
public:
	UniformGrid(unsigned int width, unsigned int height)
    : 
    Grid(width*height), 
    width(width), 
    height(height)
  {
    scaledImageIndex = new int[width * height];
    gridCoordinates = new Vector2<int>[width * height];

    CameraInfo& cameraInfo = Platform::getInstance().theCameraInfo;
    createUniformGrid(cameraInfo.resolutionWidth, cameraInfo.resolutionHeight, width, height);
  }

  virtual ~UniformGrid()
  {
    delete[] scaledImageIndex;
    delete[] gridCoordinates;
  }

	const unsigned int width;
	const unsigned int height;


  void addPoint(Vector2<int> point, int gridX, int gridY)
  {
    ASSERT(numberOfGridPoints < maxNumberOfPoints);
    int idx = Grid::addPoint(point);
    getScaledImageIndex(gridX,gridY) = idx;
    gridCoordinates[idx] = Vector2<int>(gridX, gridY);
  }//end

  int& getScaledImageIndex(unsigned int x, unsigned int y) const
  {
    ASSERT(x < width && y < height);
    return scaledImageIndex[y * width + x];
  }

  const Vector2<int>& getGridCoordinates(unsigned int idx) const
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
  
  int* scaledImageIndex;
  Vector2<int>* gridCoordinates;

};//end class UniformGrid

#endif // __UniformGrid_H__

