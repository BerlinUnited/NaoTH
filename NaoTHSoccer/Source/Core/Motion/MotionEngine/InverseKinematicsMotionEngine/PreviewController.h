/**
* @file PreviewController.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Preview controller for the Inverted pendulum
*/

#ifndef PREVIEWCONTROLLER_H
#define	PREVIEWCONTROLLER_H

#include <list>

#include "Tools/Math/Matrix.h"

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

  void setHeight(double height);

  void control(const Vector2<double>& zmp, Vector2<double>& com, Vector2<double>& dcom, Vector2<double>& ddcom);

  void clear();
  
  void init(const Vector2<double>& com, const Vector2<double>& dcom, const Vector2<double>& ddcom);

  void push(const Vector2<double>& zmp);

  size_t previewSteps() const
  {
    return parameters->f.size();
  }
  
  bool ready() const;

private:
  void update(const std::list<double>& ref, Vector3<double>&x, double& err) const;

  const Parameters* parameters;
  unsigned int dt; // ms
  
  Vector3<double> theX; // x, x', x''
  Vector3<double> theY; // y, y' ,y''
  Vector2<double> theErr;
  std::list<double> refZMPx;
  std::list<double> refZMPy;
  std::list<double> refZMPz;

  map<int, Parameters> loadedParameters;
  unsigned int theHeight;
};

std::istream& operator >>(std::istream& ist, PreviewController::Parameters& p);

#endif	/* PREVIEWCONTROLLER_H */

