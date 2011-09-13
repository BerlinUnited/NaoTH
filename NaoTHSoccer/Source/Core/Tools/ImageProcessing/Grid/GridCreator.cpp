#include "GridCreator.h"


void GridCreator::createProbabilisticGrid(Grid& grid, const int imageWidth, const int imageHeight, const int sampleNumber)
{
  int x, y;

  for (int i = 0; i < sampleNumber; i++)
  {
    x = Math::random(imageWidth);
    y = Math::random(imageHeight);
    grid.addPoint(Vector2<int>(x, y));
  }
}//end createProbabilisticGrid


void GridCreator::createUniformGrid(UniformGrid& uniformGrid, const int imageWidth, const int imageHeight, const int gridWidth, const int gridHeight)
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
      uniformGrid.addPoint(Vector2<int>(gx, gy), x, y);
      gx += deltaX + ((restX > x)?1:0);
    }//end for
    gy += deltaY + ((restY > y)?1:0);
  }//end for


  //uniformGrid.width = gridWidth;
  //uniformGrid.height = gridHeight;
}//end createUniformGrid
