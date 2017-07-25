#ifndef _ELLIPSE_H_
#define _ELLIPSE_H_

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

  void fitPoints(std::vector<double> x, std::vector<double> y) {
    x_toFit = x;
    y_toFit = y;

    Eigen::Map<Eigen::VectorXd> vx(x.data(), x.size());
    Eigen::Map<Eigen::VectorXd> vy(y.data(), y.size());

    fitPoints(vx, vy);
  }

  double mahalanobis_distance(double x, double y) {
    double a = params[0];
    double b = params[1];
    double c = params[2];
    double xx = x-center[0];
    double yy = y-center[1];

    double d = xx*xx*a + xx*yy*b + yy*yy*c;
    return std::sqrt(std::fabs(d));
  }

  double mahalanobis_radius() {
    double a = params[0];
    double b = params[1];
    double c = params[2];

    double r = center[0]*center[0]*a + center[0]*center[1]*b + center[1]*center[1]*c;

    return std::sqrt(std::fabs(r - params[5]));
  }

  double error_to(double x, double y) {
    return std::fabs( mahalanobis_distance(x,y) - mahalanobis_radius());
  }

  void getCenter(double (&C)[2]);

  void axesLength(double (&A)[2]);

  double rotationAngle();

  //Ellipse: ax^2+bxy+cy^2+dx+fy+g=0
  double params[6];
  double center[2];
  double axes[2];
  double angle;

  std::vector<double> x_toFit;
  std::vector<double> y_toFit;
};

#endif // _ELLIPSE_H_
