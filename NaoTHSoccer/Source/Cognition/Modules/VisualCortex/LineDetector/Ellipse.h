#ifndef _ELLIPSE_H_
#define _ELLIPSE_H_

#include <iostream>
#include <vector>

#include <Tools/Math/Common.h>
#include <Tools/Math/Vector2.h>

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

  
  //NOTE: this is a test for projection. 
  // https://math.stackexchange.com/questions/475436/2d-point-projection-on-an-ellipse
  Vector2d test_project(const Vector2d& point) {
    Vector2d c = getCenter();
    Vector2d a = axesLength();

    double alpha = rotationAngle();
    Vector2d p = Vector2d(point - c).rotate(-alpha);
    
    
    //p.x *= a.y;
    //p.y *= a.x;
    double theta = std::atan2(p.y, p.x);

    // calculate the point on the ellip
    Vector2d pp(a.x*cos(theta), a.y*sin(theta));

    //double k = 1.0 / std::sqrt(a.y*a.y*cos(theta)*cos(theta) + a.x*a.x*sin(theta)*sin(theta));
    //Vector2d pp( k*a.x*a.y*cos(theta), k*a.x*a.y*sin(theta));

    return c + Vector2d(pp).rotate(alpha);
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

  Vector2d getCenter() {
    
    double a = params[0];
    double b = params[1]/2;
    double c = params[2];
    double d = params[3]/2;
    double f = params[4]/2;

    double num = b*b - a*c;
    return Vector2d((c*d-b*f) / num, (a*f-b*d) / num);
  }

  void axesLength(double (&A)[2]);

  Vector2d axesLength()
  {
    double a = params[0];
    double b = params[1]/2;
    double c = params[2];
    double d = params[3]/2;
    double f = params[4]/2;
    double g = params[5];

    double num = 2*( a*f*f + c*d*d + g*b*b - 2*b*d*f - a*c*g );

    //sd = np.sqrt( np.power(A-C,2) + 4*np.power(B,2) )
    double sd = sqrt( (a-c)*(a-c) + 4*b*b );

    double den1 = (b*b - a*c) * (sd - (a+c));
    double den2 = (b*b - a*c) * (-sd - (a+c));

    return Vector2d(sqrt(num/den1), sqrt(num/den2));
  }

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
