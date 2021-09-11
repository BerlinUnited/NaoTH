/**
* @file BodyContour.h

* The file declares a class that represents the contour of the robot's body in the image.
*
* @author Kirill Yasinovskiy
*/

#ifndef _BodyContour_h_
#define _BodyContour_h_


#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include "Tools/Debug/NaoTHAssert.h"
#include <vector>

/**
* @class BodyContour
* A class that represents the contour of the robot's body in the image.
*/
class BodyContour : public naoth::Printable
{
public:
  
  enum BodyPartID
  {
    Torso,
    UpperArmLeft,
    UpperArmRight,
    LowerArmLeft,
    LowerArmRight,
    LegLeft,
    LegRight,
    FootLeft,
    FootRight,
    numOfBodyPart
  };

  struct Cell {
    Cell():id(numOfBodyPart), occupied(false) {}
    BodyPartID id;
    bool occupied;
  };

public:
  typedef std::vector<std::vector<Cell> > Grid;

  BodyContour()
  :
    stepSize(1),
    xDensity(0),
    yDensity(0),
    timestamp(0)
  {
    reset();
  }

private:
  Grid grid;
  int stepSize;
  int xDensity;
  int yDensity;
  Vector2i cameraResolution;

public:
  unsigned int timestamp;

  void setGridSize(int step, int imageWidth, int imageHeight) 
  {
    stepSize = step;
    yDensity = imageHeight/stepSize;
    xDensity = imageWidth/stepSize;
    cameraResolution.x = imageWidth;
    cameraResolution.y = imageHeight;

    grid.resize(xDensity);
    for (size_t i = 0; i < grid.size(); i++) {
      grid[i].resize(yDensity);
    }
  }

  inline int gridWidth() const {
    return xDensity;
  }
  inline int gridHeight() const {
    return yDensity;
  }
  inline int cellSize() const {
    return stepSize;
  }

  Vector2i getCellCoord(const Vector2i& point) const
  {
    return getCellCoord(point.x, point.y);
  }

  Vector2i getCellCoord(const unsigned x, const unsigned y) const
  {
    // this is allways true for unsigned x and unsigned y
    //ASSERT(x >= 0 && y >= 0)
    ASSERT(x < (unsigned)cameraResolution.x && y < (unsigned)cameraResolution.y);
    return Vector2i(x / stepSize, y / stepSize);
  }

  inline const Grid& getGrid() const {
    return grid;
  }

  const Cell& getCellFromImageCoords(const Vector2i& point) const
  {
    return getCell(point.x/stepSize, point.y/stepSize);
  }

  const Cell& getCell(const Vector2i& point) const
  {
    return getCell(point.x, point.y);
  }

  const Cell& getCell(const unsigned x, const unsigned y) const
  {
    // this is allways true for unsigned x and unsigned y
    //ASSERT(x >= 0 && y >= 0)
    ASSERT(x < (unsigned)xDensity && y < (unsigned)yDensity);
    return grid[x][y];
  }

  void setCell(int x, int y, BodyPartID id, bool value)
  {
    ASSERT(x > -1 && y > -1 && x < xDensity && y < yDensity);
    grid[x][y].id = id;
    grid[x][y].occupied = value;
  }

  bool isOccupied(const unsigned x, const unsigned y) const
  {
    if (grid.empty()) {
      return false;
    }

    ASSERT(x < (unsigned)cameraResolution.x);
    ASSERT(y < (unsigned)cameraResolution.y);

    return grid[x/stepSize][y/stepSize].occupied;
  }

  bool isOccupied(const Vector2i& point) const
  {
    return isOccupied(point.x, point.y);
  }

  Vector2i getFirstFreeCell(const Vector2i& start) const
  {
    ASSERT(start.x >= 0 && start.x < cameraResolution.x && start.y >= 0 && start.y < cameraResolution.y);
    Vector2i cell_coord(start.x/stepSize, start.y/stepSize);
    
    // do nothing if the cell is free
    if(!grid[cell_coord.x][cell_coord.y].occupied) {
      return start;
    }

    for(; cell_coord.y >= 0; cell_coord.y--) {
      if(!grid[cell_coord.x][cell_coord.y].occupied) {
        return Vector2i(start.x, (cell_coord.y+1)*cellSize()); // lower border of the cell
      }
    }

    return Vector2i(start.x, 0);
  }

  void reset()
  {
    for(size_t i = 0; i < grid.size(); i++) {
      for (size_t j = 0; j < grid[i].size(); j++) {
        grid[i][j].occupied = false;
        grid[i][j].id = BodyContour::numOfBodyPart;
      }
    }
  }

  virtual void print(std::ostream& stream) const
  {
    stream << "BodyContour" << '\n';
  }

};

class BodyContourTop : public BodyContour
{
public:
  virtual ~BodyContourTop() {}
};

#endif //_BodyContour_h_
