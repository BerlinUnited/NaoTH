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
#include <Tools/DataStructures/Printable.h>

#include <vector>
#include <map>
#include <list>


class RadarGrid: public naoth::Printable
{
public:
  //default constructor:
  RadarGrid();
  //default destructor:
  virtual ~RadarGrid(){}
  
  bool obstacleWasSeen;

  //some functions
  //get/set model
  void set(Vector2<double>);
  Vector2<double> get(double angle) const;

  void checkValid() ;

  //age the model
  void ageGrid();

  //update the model:
  //apply the odometry data, erase old values
  void updateGrid(Pose2D& odometryDelta);


  //set model params:
  void setCenter(Vector2<double>& newCenter){this->center = newCenter;}
  void setFarUpdate(double& newFarUpdate){this->nearUpdate = newFarUpdate;}
  void setNearUpdate(double& setNearUpdate){this->farUpdate = setNearUpdate;}

  void drawFieldContext();
  void drawImageContext();

  virtual void print(std::ostream& stream) const
  {
    newMap::const_iterator CIT = cells.begin();
    for (; CIT != cells.end(); ++CIT)
    {
      stream << "ValueNr.: " << CIT->first << " value: " << CIT->second.value << endl;
    }
  }//end print

private:
  //angle resolution of the grid:
  double angleResolution;

  struct Cell
  {
    //in polar coordinates system
    //value.x = distance;
    //value.y = angle
    Vector2d value;
    //age of the cell
    //in some entities
    int age;
  };

  //new version: now we are using map
  typedef std::map<unsigned int, Cell> newMap;
  newMap cells;

  //center of the grid
  Vector2<double> center;

  //model update parameters
  double nearUpdate;
  double farUpdate;

  Vector2d applyOdometry(Vector2d someValue, Pose2D& odometryDelta);

 
  unsigned int getIndexByAngle(const double& angle) const;
};

#endif// __RadarGrid_h_
