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
#include "Tools/Math/Vector2.h"

class PreviewController
{
public:

  // precalculated values
  Matrix3x3<double> A;
  Vector3d b;
  Vector3d c;

  // loaded values
  struct Parameters
  {
    static double length_f;
    // for preview control
    double Ki;
    Vector3<double> K;
    std::list<double> f;
  };

  PreviewController();

  void loadParameter();

  //
  // state of the controller

  void init(const Vector3d& com, const Vector2<double>& dcom, const Vector2<double>& ddcom);
  
  void control(Vector3d& com, Vector2<double>& dcom, Vector2<double>& ddcom);
  
  size_t previewSteps() const { return parameters->f.size(); }  

  bool ready() const;
  
  //
  // input ZPM handling

  void push(const Vector3d& zmp);
  Vector3d front() const;
  Vector3d back() const;
  unsigned int count() const { return refZMPx.size(); }
  void clear();


private:
  void setHeight(double height);
  void update(const std::list<double>& ref, Vector3<double>&x, double& err) const;

  // output: internal state of the contsoller
  Vector3<double> theX; // x, x', x''
  Vector3<double> theY; // y, y' ,y''
  double theZ; // z
  Vector2<double> theErr;
  
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
};

std::istream& operator >>(std::istream& ist, PreviewController::Parameters& p);

#endif  /* _PREVIEWCONTROLLER_H */

