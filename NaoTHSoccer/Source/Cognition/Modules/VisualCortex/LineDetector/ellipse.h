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

  void fitPoints(double x[],  double y[], int size) {
    Eigen::Map<Eigen::VectorXd> vx(x, size);
    Eigen::Map<Eigen::VectorXd> vy(y, size);

    fitPoints(vx, vy);
  }

  double error_to(double x, double y) {
    // Ax^2+Bxy+Cy^2+Dx+Ey+F=0
    return std::fabs(params[0]*x*x
      + params[1]*x*y
      + params[2]*y*y
      + params[3]*x
      + params[4]*y
      + params[5]);
  }

  void getCenter(double (&C)[2]) {
    double a = params[0];
    double b = params[1]/2;
    double c = params[2];
    double d = params[3]/2;
    double f = params[4]/2;

    double num = b*b - a*c;
    C[0] = (c*d-b*f) / num;
    C[1] = (a*f-b*d) / num;
  }

  //Ellipse: ax^2+bxy+cy^2+dx+fy+g=0
  double params[5];
};

#endif // ELLIPSE_H
