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
  void set(double angle, double value);
  void set(Vector2<double>);
  void get(double angle, double& value);

  //set model params:
  void setCenter(Vector2<double>& newCenter){this->center = newCenter;}
  void setFarUpdate(double& newFarUpdate){this->nearUpdate = newFarUpdate;}
  void setNearUpdate(double& setNearUpdate){this->farUpdate = setNearUpdate;}


  void drawFieldContext();
  void drawImageContext();

  //reset model
  void reset();

  virtual void print(std::ostream& stream) const
  {
    //int i = 0;
    //std::vector<double>::const_iterator RGI = values.begin();
    //for (; RGI != values.end(); ++RGI)
    for (int i = 0; i < 18; ++i)
    {
      stream << "ValueNr.: " << i << " value: " << values[i] << endl;
    }
  }//end print

private:
  //angle resolution of the grid:
  double angleResolution;
  //values of the grid
  //std::vector<double> values;
  double values[18];

  //center of the grid
  Vector2<double> center;

  //model update parameters
  double nearUpdate;
  double farUpdate;
 
  int getIndexByAngle(const double& angle);
};
