/* 
 * File:   Grid.h
 * Author: Kirill Yasinovskiy
 *
 * Created on 27. Juni 2011
 */

#ifndef __RadarGrid_h_
#define __RadarGrid_h_

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
#include "Tools/Math/Matrix2x2.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/RingBuffer.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

#include "Representations/Modeling/BallModel.h"


#include <vector>
#include <map>
#include <list>
#include <algorithm>


class RadarGrid: public naoth::Printable
{
public:
  //default constructor:
  RadarGrid();
  //default destructor:
  virtual ~RadarGrid(){}

  // possible go directions for a robot
  enum Directions{ back, backRight, right, frontRight, front, frontLeft, left, backLeft, numberOfDirections };
  // search directions
  enum SearchDirections{searchLeft, searchLeftAndRight, searchRight};
  // number of sectors
  enum { numOfSectors = 45 , maxDistance = 1500};
  // update types
  enum UpdateMode{ overwrite, extend, limit };
  // number of points in buffer
  enum { bufferCapacity = 5 };
  // time, after the obstacle is forgotten
  enum { timeToForget = 6000, timeToForgetWithBuffer = 1200 };

  //angle resolution of the grid:
  double angleResolution;

  // do we have some obstacle?
  bool obstacleWasSeen;

  // use buffer or just a single value
  bool useBuffer;

  //center of the grid
  Vector2<double> center;


  // store the system time
  unsigned int currentTime;

  // density of the grid:
  // how wide is a sector in degrees
  double density;

  //model update parameters
  double nearUpdate;
  double farUpdate;
  
  //some functions
  //get/set model
  void addObstaclePoint(Vector2<double>);

  // set the current system time
  void setCurrentTime(unsigned int tStamp) {this->currentTime = tStamp;}

  /** get the distance for a particular angle */
  double getDistanceForAngle(double angle) const;

  /** returns the distance to the closest obstacle
   * in the corridor specified by angle and width */
  double getDistanceInCorridor(double angle, double width) const;

  /** get all obstacles from the model in one vector of
  * of positions
  */
  void getAllObstacles(std::vector<Vector2d> obstaclesVector) const;

  // check whether model is valid
  void checkValid();

  //age the model
  void ageGrid();

  //update the model:
  //apply the odometry data, erase old values
  void updateGridByOdometry(Pose2D& odometryDelta);

  //set model params:
  void setCenter(Vector2<double> newCenter){this->center = newCenter;}
  void setFarUpdate(double newFarUpdate){this->nearUpdate = newFarUpdate;}
  void setNearUpdate(double setNearUpdate){this->farUpdate = setNearUpdate;}

  // debug drawings
  void drawFieldContext();
  void drawImageContext();

  virtual void print(std::ostream& stream) const
  {
    stream << "Obstacle was seen: " << obstacleWasSeen << endl;
    cellsMap::const_iterator it = cells.begin();
    for (; it != cells.end(); ++it)
    {
      stream << "ValueNr.: " << it->first << " mean: " << it->second.mean << endl;
      stream << "SumOfBuffer.: " << it->second.sum << endl;
      for (int i = 0; i < it->second.buffer.getNumberOfEntries(); i++)
      {
        stream << "\t" << "bufferNr: " << i << " : " << it->second.buffer[i] << " time: " << it->second.timeStamp << endl;
      }
    }
  }//end print

private:
  struct Cell
  {
    Vector2d mean;
    //buffer for percepts
    RingBuffer<Vector2d, bufferCapacity> buffer;
    //sum of the buffer
     Vector2d sum;
    //time the cell was set
    unsigned int timeStamp;
    Cell():
      mean(0.0, 0.0),
      sum(0.0, 0.0),
      timeStamp(0)
    {}
  };

  //new version: now we use map
  typedef std::map<int, Cell> cellsMap;
  cellsMap cells;


  Vector2d applyOdometry(Vector2d& someValue, Pose2D& odometryDelta);

  void setMean(int& position, Vector2d& newValue);
 
  // get sector of model for a given angle
  inline int getSectorByAngle(const double angle) const;

  // get angle of a given sector
  inline double getAngleBySector(const int sector) const;

  // get sum and average values of buffer
  inline void getSumAndAverage(RingBuffer<Vector2d, 5>& buffer, Vector2d& sum, Vector2d& average) const;
};


#endif// __RadarGrid_h_
