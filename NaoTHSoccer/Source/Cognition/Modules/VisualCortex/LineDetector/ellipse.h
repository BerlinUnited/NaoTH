#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <iostream>
#include <vector>

#include <Tools/naoth_eigen.h>

class Ellipse
{
public:
  Ellipse();

  void fitPoints(Eigen::VectorXd x,  Eigen::VectorXd y);

  //Ellipse: ax^2+bxy+cy^2+dx+fy+g=0
  double params[5];
};

#endif // ELLIPSE_H
