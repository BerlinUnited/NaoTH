#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <iostream>
#include <vector>

#include <Tools/Math/Common.h>

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
    double b = params[1]/2;

    double xx = center[0];
    double yy = center[1];

    double r = ( ( (xx * params[0]) + (yy * b) ) * xx) +
               ( ( (xx * b) + (yy * params[2]) ) * yy);

    r = std::fabs(params[5]) + std::fabs(r);

    /*
     * (x-m)^T * A * (x-m)
     *
     * A = ( a  b/2
     *      b/2  c )
     */

    xx = x-center[0];
    yy = y-center[1];


    double d = ( ( (xx * params[0]) + (yy * b) ) * xx) +
               ( ( (xx * b) + (yy * params[2]) ) * yy);

    // percentage
    return std::fabs(d - r)/r;
  }

  void getCenter(double (&C)[2]);

  void axesLength(double (&A)[2]);

  double rotationAngle();

  //Ellipse: ax^2+bxy+cy^2+dx+fy+g=0
  double params[5];
  double center[2];
  double axes[2];
  double angle;
};

#endif // ELLIPSE_H
