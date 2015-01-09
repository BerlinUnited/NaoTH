/** 
* @file ReachibilityGrid.h
* Declaration of class ReachibilityGrid.
*
* @author <A href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</A>
*/

#ifndef _ReachibilityGrid_h_
#define _ReachibilityGrid_h_

#include "PlatformInterface/Platform.h"

#include <string>
#include <fstream>

/**
* @class ReachibilityGrid
*
* 
*/
class ReachibilityGrid
{
public:
  /** Constructor */
  ReachibilityGrid()
  {
    reset();

    const std::string& dirlocation = naoth::Platform::getInstance().theConfigDirectory;
    std::string path = dirlocation + "/reachability_grid.dat";
    loadFromFile(path);
  }

  ReachibilityGrid(const std::string& file_name)
  {
    reset();

    const std::string& dirlocation = naoth::Platform::getInstance().theConfigDirectory;
    std::string path = dirlocation + "/general/" + file_name;
    loadFromFile(path);
  }

  virtual ~ReachibilityGrid(){}

  void reset()
  {
    for(int x = 0; x < resolution; x++)
      for(int y = 0; y < resolution; y++)
        for(int z = 0; z < resolution; z++)
    {
      reachibility_grid[x][y][z] = 0;
    }//end for
  }//end init

  int getReachibility(Vector3<double> point)
  {
    int x = (int)((point.x+offset + 0.5)*0.1);
    int y = (int)((point.y+offset + 0.5)*0.1);
    int z = (int)((point.z+offset + 0.5)*0.1);
    if( 0 < x && x < resolution &&
        0 < y && y < resolution &&
        0 < z && z < resolution)
      return reachibility_grid[x][y][z];

    return 0;
  }//end getReachibility

  /** 
  * 
  */
  static const int offset = 300;
  static const int resolution = 64;
  int reachibility_grid[resolution][resolution][resolution];

  bool loadFromFile(std::string fileName)
  {
    std::ifstream inputFileStream ( fileName.c_str() , std::ifstream::in | std::ifstream::binary );
    
    if(inputFileStream.fail())
    {
      // could not open reachability grid
      std::cerr << "ReachibilityGrid cannot be load from " << fileName << ". No Configs found. Do you call ../bin/naoth from ~/naoqi?" << std::endl;
      ASSERT(false);
      return false;
    }//end if

    for(int x = 0; x < resolution; x++)
      for(int y = 0; y < resolution; y++)
        for(int z = 0; z < resolution; z++)
    {
      inputFileStream >> reachibility_grid[x][y][z];
    }//end for

    //inputFileStream.read((char*)&(reachibility_grid), sizeof(reachibility_grid));
    inputFileStream.close();

    return true;
  }//end loadFromFile
};


class TransformedReachibilityGrid
{
public:
  TransformedReachibilityGrid(const ReachibilityGrid& grid)
    : grid(grid),
      kickingFoot(1) // left foor by default
  {
  }

  virtual ~TransformedReachibilityGrid(){}

  const ReachibilityGrid& grid;

  void setTransformation(const Pose3D& other)
  {
    transformation = other;
  }//end setTransformation

  void setRightFoot(bool rightFoot)
  {
    if(rightFoot)
      kickingFoot = -1;
    else
      kickingFoot = 1;
  }//end setRightFoot

  const Pose3D& getTransformation() const
  {
    return transformation;
  }//end setTransformation

  bool gridPointReachable(const Vector3<int>& gridPoint) const
  {
    return 
      gridPoint.x >= 0 && gridPoint.x < grid.resolution &&
      gridPoint.y >= 0 && gridPoint.y < grid.resolution &&
      gridPoint.z >= 0 && gridPoint.z < grid.resolution &&
      grid.reachibility_grid[gridPoint.x][gridPoint.y][gridPoint.z] > 0;
  }//end gridPointReachable

  Vector3<double> gridPointToWorldCoordinates(const Vector3<int>& gridPoint) const
  {
    // get the chest coordinates
    const Pose3D& chest = getTransformation();

    // apply the offset
    Vector3<double> offset(-ReachibilityGrid::offset, -ReachibilityGrid::offset, -ReachibilityGrid::offset);
    Vector3<double> pointCoord = offset + gridPoint*10;
    
    // mirror according to the current kicking foot
    pointCoord.y *= -kickingFoot;

    //transform in the chest coordinates
    return chest*pointCoord;
  }//end gridPointToWorldCoordinates

  Vector3<int> pointToGridCoordinates(const Vector3<double>& worldPoint) const
  {
    // get the chest coordinates
    const Pose3D& chest = getTransformation();

    Vector3<double> localPoint = chest.invert()*worldPoint;
    
    // mirror according to the current kicking foot
    localPoint.y *= -kickingFoot;

    // apply the offset
    Vector3<double> offset(-ReachibilityGrid::offset, -ReachibilityGrid::offset, -ReachibilityGrid::offset);
    Vector3<double> gridCoords = (localPoint-offset)*0.1;

    //transform in the chest coordinates
    return Vector3<int>(
      (int)Math::round(gridCoords.x), 
      (int)Math::round(gridCoords.y), 
      (int)Math::round(gridCoords.z));
  }//end pointToGridCoordinates

private:
  Pose3D transformation;
  int kickingFoot;
  
};

#endif   //  _ReachibilityGrid_h_
