/**
 * @file PreviewController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Preview controller for the Inverted pendulum
 */

#include <fstream>
#include <map>
#include <sstream>
#include <iostream>

#include "PreviewController.h"
#include <PlatformInterface/Platform.h>
#include <Tools/DataConversion.h>

PreviewController::PreviewController()
:parameters(NULL),
theHeight(0)
{
}

void PreviewController::setHeight(double height)
{
  unsigned int newHeight = (unsigned int)Math::round(height);
  newHeight = Math::clamp(newHeight, 200u, 300u);
  
  if ( theHeight != newHeight )
  {
    theHeight = newHeight;
    if ( loadedParameters.find(theHeight) == loadedParameters.end() )
    {
      string path =naoth::Platform::getInstance().theConfigDirectory + "scheme/"
            + naoth::Platform::getInstance().theScheme + "/previewControl/";
      path += DataConversion::toStr(theHeight)+".prm";
      cout<<"PreviewController load "<<path<<endl;
    
      ifstream ifs(path.c_str());
      ASSERT(ifs.good());
      ifs >> loadedParameters[theHeight];
    }
    parameters = &(loadedParameters[theHeight]);
  }
}

void PreviewController::update(const std::list<double>& ref, Vector3<double>&x, double& err) const
{
  double u = -parameters->Ki * err - parameters->K*x;
  std::list<double>::const_iterator refi = ref.begin();
  std::list<double>::const_iterator fi = parameters->f.begin();
  for (; refi != ref.end() && fi != parameters->f.end(); ++fi, ++refi)
  {
    u -= (*fi) * (*refi);
  }

  x = parameters->A * x + parameters->b*u;
  double zmp = parameters->c*x;
  err += (zmp - ref.front());
}

bool PreviewController::ready() const
{
  if (parameters == NULL)
    return false;
    
  size_t size = previewSteps() - 1;
  return refZMPx.size() >= size && refZMPy.size() >= size;
}

void PreviewController::control(Vector3<double>& com, Vector2<double>& dcom, Vector2<double>& ddcom)
{
  ASSERT(ready());
  double h = refZMPz.front();
  setHeight(h);
  update(refZMPx, theX, theErr.x);
  update(refZMPy, theY, theErr.y);
  refZMPx.pop_front();
  refZMPy.pop_front();
  refZMPz.pop_front();

  com.x = theX[0];
  dcom.x = theX[1];
  ddcom.x = theX[2];
  com.y = theY[0];
  dcom.y = theY[1];
  ddcom.y = theY[2];
  com.z = h;
}

void PreviewController::clear()
{
  refZMPx.clear();
  refZMPy.clear();
  refZMPz.clear();
  theErr = Vector2<double>(0, 0);
}

void PreviewController::init(const Vector3d& com, const Vector2<double>& dcom, const Vector2<double>& ddcom)
{
  setHeight(com.z);

  theX[0] = com.x;
  theX[1] = dcom.x;
  theX[2] = ddcom.x;

  theY[0] = com.y;
  theY[1] = dcom.y;
  theY[2] = ddcom.y;
}

void PreviewController::push(const Vector3d& zmp)
{
  refZMPx.push_back(zmp.x);
  refZMPy.push_back(zmp.y);
  refZMPz.push_back(zmp.z);
}

Vector3d PreviewController::front() const
{
  return Vector3d(refZMPx.front(), refZMPy.front(), refZMPz.front());
}

Vector3d PreviewController::back() const
{
  return Vector3d(refZMPx.back(), refZMPy.back(), refZMPz.back());
}

std::istream & operator >>(std::istream& ist, PreviewController::Parameters& p)
{
  ist >> p.A >> p.b >> p.c >> p.Ki >> p.K;

  double v;
  while (!ist.eof())
  {
    ist >> v;
    p.f.push_back(v);
  }
  return ist;
}
