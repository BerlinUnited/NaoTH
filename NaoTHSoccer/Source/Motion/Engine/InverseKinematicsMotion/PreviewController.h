/**
* @file PreviewController.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Preview controller for the Inverted pendulum
*/

#ifndef _PREVIEWCONTROLLER_H
#define _PREVIEWCONTROLLER_H

#include <list>
#include <vector>
#include <map>

#include "Tools/Math/Matrix3x3.h"
#include "Tools/Math/Vector2.h"

class PreviewController
{
private:
  // precalculated values
  Matrix3x3<double> A;
  Vector3d b;
  Vector3d c;

  // loaded values
  struct Parameters
  {
    // for preview control
    double Ki;
    Vector3d K;
    std::vector<double> f;
  };

public:
  PreviewController();
  void loadParameter();

  //
  // state of the controller

  void init(const Vector3d& com, const Vector2d& dcom, const Vector2d& ddcom);
  
  void control(Vector3d& com, Vector2d& dcom, Vector2d& ddcom);
  
  size_t previewSteps() const { return parameters->f.size(); }

  bool ready() const;
  
  //
  // input ZPM handling
  void push(const Vector3d& zmp);
  void pop();
  Vector3d front() const;
  Vector3d back() const;
  unsigned int count() const { return static_cast<unsigned int> (refZMPx.size()); }
  void clear();

  Vector3d com() const {
    return Vector3d(theX[0], theY[0], theZ);
  }

  Vector2d com_velocity() const {
    return Vector2d(theX[1], theY[1]);
  }

  Vector2d com_acceleration() const {
    return Vector2d(theX[2], theY[2]);
  }

  // NOTE: experimental
  void setState(const Vector3d& com, const Vector2d& dcom, const Vector2d& ddcom) {
    setHeight(com.z);

    theX[0] = com.x;
    theX[1] = dcom.x;
    theX[2] = ddcom.x;

    theY[0] = com.y;
    theY[1] = dcom.y;
    theY[2] = ddcom.y;
  }

private:
  void setHeight(double height);
  void update(const std::list<double>& ref, Vector3d&x, double& err) const;

  // output: internal state of the contsoller
  Vector3d theX; // x, x', x''
  Vector3d theY; // y, y', y''
  double theZ; // z
  Vector2d theErr;
  bool initialized;
  
  // input: reference ZMP
  std::list<double> refZMPx;
  std::list<double> refZMPy;
  std::list<double> refZMPz;


  // parameters for diffent heights
  typedef std::map<int, Parameters> ParameterMap;
  ParameterMap loadedParameters;
  // pinter to the parameters corresponding to the current height 
  // i.e., parameters->loadedParameters[parameterHeight]
  const Parameters* parameters;
  // current height used for the parameters
  unsigned int parameterHeight;
  // time step of the precalculated parameters
  double parameterTimeStep;
};

#endif  /* _PREVIEWCONTROLLER_H */

