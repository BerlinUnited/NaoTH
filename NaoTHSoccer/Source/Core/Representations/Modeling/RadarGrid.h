/* 
 * File:   Grid.h
 * Author: Kirill Yasinovskiy
 *
 * Created on 27. Juni 2011
 */

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Pose2D.h"
#include <Tools/DataStructures/Printable.h>

#include <vector>


class RadarGrid: public naoth::Printable
{
public:
  //default constructor:
  RadarGrid();
  //default destructor:
  virtual ~RadarGrid(){}
  

  //some functions
  //get/set model
  void set(Vector2<double>);
  void get(double angle, Vector2<double>& value);

  //age the model
  void ageGrid();

  //update the model:
  //apply the odometry data, erase old values
  void updateGrid(Pose2D& odometryDelta);
  Vector2<double> applyOdometry(int& index, Pose2D& odometryDelta);


  //set model params:
  void setCenter(Vector2<double>& newCenter){this->center = newCenter;} 
  void setFarUpdate(double& newFarUpdate){this->nearUpdate = newFarUpdate;} 
  void setNearUpdate(double& setNearUpdate){this->farUpdate = setNearUpdate;} 

  void drawFieldContext();
  void drawImageContext();

  //reset cell
  void resetCell(int& index);

  virtual void print(std::ostream& stream) const
  {
    //int i = 0;
    //std::vector<double>::const_iterator RGI = values.begin();
    //for (; RGI != values.end(); ++RGI)
    for (int i = 0; i < 18; ++i)
    {
      stream << "ValueNr.: " << i << " value: " << values[i].value << endl;
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
    Vector2<double> value;
    //age of the cell
    //in some entities
    int age;
  };

  //values of the grid
  //std::vector<double> values;
  std::vector<Cell> values;

  //center of the grid
  Vector2<double> center;

  Pose2D offset;

  //model update parameters
  double nearUpdate;
  double farUpdate;
 
  int getIndexByAngle(const double& angle);
};
