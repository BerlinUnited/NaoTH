/**
 * @file PreviewController.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Preview controller for the Inverted pendulum
 */

#include <map>

#include "PreviewController.h"
#include <PlatformInterface/Platform.h>
#include <Tools/DataConversion.h>

using namespace std;

PreviewController::PreviewController()
  :
  parameters(NULL),
  parameterHeight(0)
{
  loadParameter();

  double dt = parameterTimeStep; // 10ms

  A.c[0] = Vector3d(1      , 0 , 0);
  A.c[1] = Vector3d(dt     , 1 , 0);
  A.c[2] = Vector3d(dt*dt/2, dt, 1);

  b = Vector3d(dt*dt*dt/6, dt*dt/2, dt);

  setHeight(200); // some initial height

  initialized = false;
}

void PreviewController::setHeight(double height)
{
  unsigned int newHeight = (unsigned int)Math::round(height);
  // todo: it's bad if this happens
  newHeight = Math::clamp(newHeight, 200u, 300u);
  
  if ( parameterHeight != newHeight )
  {
    std::cout << "[PreviewController] change height from " << parameterHeight << " to " << newHeight << std::endl;
    parameterHeight = newHeight;

    double z = parameterHeight * 0.001; // height in m
    c = Vector3d(1,0,-z/Math::g);
    
    ParameterMap::const_iterator iter = loadedParameters.find(parameterHeight);
    assert(iter != loadedParameters.end());
    parameters = &(iter->second);
  }
}

void PreviewController::loadParameter()
{
  // generate the file name
  string path = naoth::Platform::getInstance().theConfigDirectory; 
  path += "platform/";
  path += naoth::Platform::getInstance().thePlatform;
  path += "/previewControl.prm";
    
  // open the stream
  ifstream ifs(path.c_str());
  if(!ifs.good()){
    THROW("[PreviewController] ERROR: load " << path << endl);
  } else {
    std::cout << "[PreviewController] load " << path << endl;
  }
      
  // read the header
  double length_f(0.0), number_of_entries(0.0);
  ifs >> parameterTimeStep >> length_f >> number_of_entries;

  // read the parameters for each height step
  double height(0.0);
  for(int i = 0; i < number_of_entries; i++)
  {
    ifs >> height;
    
    PreviewController::Parameters& p = loadedParameters[(int)height];
    p.f.resize((int)length_f);

    ifs >> p.Ki >> p.K;
    
    for(unsigned int i = 0; i < p.f.size(); i++) {
      assert(!ifs.eof());
      ifs >> p.f[i];
    }
  }
}//end loadParameter


void PreviewController::update(const std::list<double>& ref, Vector3d&x, double& err) const
{
  // u = -Ki*err - Ks*X - F*P;

  if(initialized) {
    double zmp = c*x;
    err += (zmp - ref.front());
  }

  double u = -parameters->Ki * err - parameters->K*x;
  
  // the preview length is limited by the nomber of parameters and
  // by the length of the reference zmp (ref)
  std::list<double>::const_iterator refi = ref.begin();
  std::vector<double>::const_iterator fi = parameters->f.begin();
  for (; refi != ref.end() && fi != parameters->f.end(); ++fi, ++refi)
  {
    u -= (*fi) * (*refi);
  }

  x = A * x + b*u;

  //double zmp = c*x;
  //err += (zmp - ref.front());
}

void PreviewController::control(Vector3d& com, Vector2d& dcom, Vector2d& ddcom)
{
  ASSERT(ready());

  theZ = refZMPz.front();
  setHeight(theZ);
  
  update(refZMPx, theX, theErr.x);
  update(refZMPy, theY, theErr.y);
  
  initialized = true;

  //refZMPx.pop_front();
  //refZMPy.pop_front();
  //refZMPz.pop_front();

  // copy results
  com.x = theX[0];
  dcom.x = theX[1];
  ddcom.x = theX[2];

  com.y = theY[0];
  dcom.y = theY[1];
  ddcom.y = theY[2];

  com.z = theZ;
}


void PreviewController::init(const Vector3d& com, const Vector2d& dcom, const Vector2d& ddcom)
{
  setState(com, dcom, ddcom);
}

bool PreviewController::ready() const
{
  if (parameters == NULL) {
    return false;
  }
    
  size_t size = previewSteps() - 1; // TODO: why -1?
  return refZMPx.size() >= size && refZMPy.size() >= size;
}

void PreviewController::clear()
{
  refZMPx.clear();
  refZMPy.clear();
  refZMPz.clear();
  initialized = false;
  theErr = Vector2d(0, 0);
}

void PreviewController::push(const Vector3d& zmp)
{
  refZMPx.push_back(zmp.x);
  refZMPy.push_back(zmp.y);
  refZMPz.push_back(zmp.z);
}

void PreviewController::pop()
{
  refZMPx.pop_front();
  refZMPy.pop_front();
  refZMPz.pop_front();
}

Vector3d PreviewController::front() const
{
  return Vector3d(refZMPx.front(), refZMPy.front(), refZMPz.front());
}

Vector3d PreviewController::back() const
{
  return Vector3d(refZMPx.back(), refZMPy.back(), refZMPz.back());
}
