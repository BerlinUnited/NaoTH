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

  using CellCoord = Vector2<unsigned int>;

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
  unsigned int stepSize;
  unsigned int xDensity;
  unsigned int yDensity;
  Vector2<unsigned int> cameraResolution;

public:
  unsigned int timestamp;

  void setGridSize(unsigned int step, unsigned int imageWidth, unsigned int imageHeight) {
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

  inline unsigned int gridWidth() const {
    return xDensity;
  }

  inline unsigned int gridHeight() const {
    return yDensity;
  }

  inline unsigned int cellSize() const {
    return stepSize;
  }

  inline const Grid& getGrid() const {
    return grid;
  }

  CellCoord getCellCoordFromImageCoords(const Vector2i& point) const {
    return getCellCoordFromImageCoords(static_cast<unsigned>(point.x),
                                       static_cast<unsigned>(point.y));
  }

  CellCoord getCellCoordFromImageCoords(const unsigned x, const unsigned y) const {
    return CellCoord(x/stepSize, y/stepSize);
  }

  const Cell& getCellFromImageCoords(const Vector2i& point) const {
    return getCell(static_cast<unsigned int>(point.x)/stepSize,
                   static_cast<unsigned int>(point.y)/stepSize);
  }

  const Cell& getCell(const CellCoord& coord) const {
    return getCell(coord.x, coord.y);
  }

  const Cell& getCell(const unsigned x, const unsigned y) const {
    return grid.at(x).at(y);
  }

  void setCell(unsigned int x, unsigned int y, BodyPartID id, bool occupied) {
    grid.at(x).at(y).id = id;
    grid[x][y].occupied = occupied;
  }

  inline bool isOccupied(const unsigned x, const unsigned y) const {
    if (grid.empty()) {
      return false;
    }

    return grid.at(x/stepSize).at(y/stepSize).occupied;
  }

  bool isOccupied(const CellCoord& coord) const {
    return isOccupied(coord.x, coord.y);
  }

  Vector2i getImageCoordsOfFirstFreeCell(const Vector2i& start) const {
    CellCoord cell_coord(static_cast<unsigned int>(start.x)/stepSize,
                         static_cast<unsigned int>(start.y)/stepSize);
    
    // do nothing if the cell is free
    if(!grid.at(cell_coord.x).at(cell_coord.y).occupied) {
      return start;
    }

    for(unsigned int y = 0; y <= cell_coord.y; ++y) {
      if(!grid[cell_coord.x][cell_coord.y-y].occupied) {
        return Vector2i(start.x, static_cast<int>((cell_coord.y+1)*cellSize())); // lower border of the cell
      }
    }

    return Vector2i(start.x, 0);
  }

  void reset() {
    for(size_t i = 0; i < grid.size(); i++) {
      for (size_t j = 0; j < grid[i].size(); j++) {
        grid[i][j].occupied = false;
        grid[i][j].id = BodyContour::numOfBodyPart;
      }
    }
  }

  virtual void print(std::ostream& stream) const {
    stream << "BodyContour" << '\n';
  }
};

class BodyContourTop : public BodyContour
{
public:
  virtual ~BodyContourTop() {}
};

#endif //_BodyContour_h_
