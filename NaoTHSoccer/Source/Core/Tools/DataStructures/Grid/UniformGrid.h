#ifndef _UniformGrid_H_
#define _UniformGrid_H_

#include <vector>
#include <Tools/Math/Vector2.h>

class UniformGrid
{
public:
  UniformGrid(unsigned int imageWidth, unsigned int imageHeight, unsigned int gridWidth, unsigned int gridHeight)
    :
    width(gridWidth),
    height(gridHeight),
    gridCoordinates(gridWidth * gridHeight),
    pointsCoordinates(gridWidth * gridHeight)
  {
    setDimensions(imageWidth, imageHeight, width, height);
  }

  virtual ~UniformGrid(){}

  // dimensions of the grid
  const unsigned int width;
  const unsigned int height;

  inline unsigned int size() const {
    return width * height;
  }

  inline int getScaledImageIndex(unsigned int x, unsigned int y) const {
    ASSERT(x < width && y < height);
    return y * width + x;
  }

  inline const Vector2<int>& getGridCoordinates(unsigned int i) const {
    ASSERT(i < gridCoordinates.size());
    return gridCoordinates[i];
  }

  inline const Vector2<int>& getPoint(unsigned int i) const {
    ASSERT(i < pointsCoordinates.size());
    return pointsCoordinates[i];
  }

private:
  void setDimensions(const int imageWidth, const int imageHeight, const int gridWidth, const int gridHeight)
  {
    const int deltaX = imageWidth / gridWidth;
    const int deltaY = imageHeight / gridHeight;

    const int offsetX = deltaX / 2;
    const int offsetY = deltaY / 2;

    const int restX = imageWidth % gridWidth;
    const int restY = imageHeight % gridHeight;

    Vector2i ip; // image point
    Vector2i gp; // grid point

    ip.y = offsetY;
    for(gp.y = 0; gp.y < gridHeight; gp.y++)
    {
      ip.x = offsetX;

      for(gp.x = 0; gp.x < gridWidth; gp.x++) {
        addPoint(ip, gp);
        ip.x += deltaX + ((restX > gp.x) ? 1 : 0);
      }
      ip.y += deltaY + ((restY > gp.y) ? 1 : 0);
    }
  }//end createUniformGrid

  inline void addPoint(const Vector2i& imagePoint, const Vector2i& gridPoint)
  {
    int idx = getScaledImageIndex(gridPoint.x, gridPoint.y);// ensures 0 <= idx < width*height

    pointsCoordinates[idx] = imagePoint;
    gridCoordinates[idx] = gridPoint;
  }

private:
  std::vector<Vector2<int> > gridCoordinates;
  std::vector<Vector2<int> > pointsCoordinates;

};//end class UniformGrid

#endif // _UniformGrid_H__

