/**
* @file PreviewController.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Preview controller for the Inverted pendulum
*/

#ifndef _PREVIEWCONTROLLER_H
#define _PREVIEWCONTROLLER_H

#include <list>
#include <map>

#include "Tools/Math/Matrix3x3.h"

class PreviewController
{
public:

  struct Parameters
  {
    // for preview control
    Matrix3x3<double> A;
    Vector3<double> b;
    Vector3<double> c;
    double Ki;
    Vector3<double> K;
    std::list<double> f;
  };

  PreviewController();

  void control(Vector3d& com, Vector2<double>& dcom, Vector2<double>& ddcom);

  void clear();
  
  void init(const Vector3d& com, const Vector2<double>& dcom, const Vector2<double>& ddcom);

  void push(const Vector3d& zmp);

  Vector3d front() const;

  Vector3d back() const;

  size_t previewSteps() const
  {
    return parameters->f.size();
  }
  
  bool ready() const;

  unsigned int count() const { return refZMPx.size(); }

private:
  void setHeight(double height);

  void update(const std::list<double>& ref, Vector3<double>&x, double& err) const;

  const Parameters* parameters;
  
  Vector3<double> theX; // x, x', x''
  Vector3<double> theY; // y, y' ,y''
  Vector2<double> theErr;
  std::list<double> refZMPx;
  std::list<double> refZMPy;
  std::list<double> refZMPz;

  std::map<int, Parameters> loadedParameters;
  unsigned int theHeight;
};

std::istream& operator >>(std::istream& ist, PreviewController::Parameters& p);

#endif  /* _PREVIEWCONTROLLER_H */

