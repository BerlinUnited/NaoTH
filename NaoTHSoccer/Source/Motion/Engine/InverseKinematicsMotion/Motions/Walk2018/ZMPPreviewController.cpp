/**
* @file FootStepPlanner.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#include "ZMPPreviewController.h"
#include <PlatformInterface/Platform.h>

using namespace std;

void ZMPPreviewController::execute(){
    if( !ready() ) {
      return;
    }

    // dummy, not used
    Vector3d com;
    Vector2d dcom, ddcom;
    control(com, dcom, ddcom);
    getZMPReferenceBuffer().pop();

    getTargetCoMFeetPose().pose.com.translation = com;

    PLOT("ZMPPreviewController:is_stationary", is_stationary());
    PLOT("ZMPPreviewController:com:x", com.x);
    PLOT("ZMPPreviewController:com:y", com.y);
    PLOT("ZMPPreviewController:dcom:x", dcom.x);
    PLOT("ZMPPreviewController:dcom:y", dcom.y);
    PLOT("ZMPPreviewController:dcom:abs", dcom.abs());
    PLOT("ZMPPreviewController:ddcom:x", ddcom.x);
    PLOT("ZMPPreviewController:ddcom:y", ddcom.y);
    PLOT("ZMPPreviewController:ddcom:abs", ddcom.abs());
}

ZMPPreviewController::ZMPPreviewController():
    parameters(getWalk2018Parameters().zmpPreviewControllerParams)
{
  double dt = parameters.parameterTimeStep; // 10ms

  A.c[0] = Vector3d(1      , 0 , 0);
  A.c[1] = Vector3d(dt     , 1 , 0);
  A.c[2] = Vector3d(dt*dt/2, dt, 1);

  b = Vector3d(dt*dt*dt/6, dt*dt/2, dt);

  parameters.update(200); // some initial height
}

void ZMPPreviewController::setHeight(double height) {
  int newHeight = static_cast<int>(Math::round(height));
  // todo: it's bad if this happens
  newHeight = Math::clamp(newHeight, 200, 300);

  if ( parameters.current->height != newHeight )
  {
    // std::cout << "[PreviewController] change height from " << parameters.current->height << " to " << newHeight << std::endl;
    parameters.update(newHeight);

    double z = newHeight * 0.001; // height in m
    c = Vector3d(1,0,-z/Math::g);
  }
}

void ZMPPreviewController::update(const std::list<double>& ref, Vector3d&x, double& err) const {
  // u = -Ki*err - Ks*X - F*P;

  double u = -parameters.current->Ki * err - parameters.current->K*x;

  // the preview length is limited by the nomber of parameters and
  // by the length of the reference zmp (ref)
  std::list<double>::const_iterator refi = ref.begin();
  std::vector<double>::const_iterator fi = parameters.current->f.begin();
  for (; refi != ref.end() && fi != parameters.current->f.end(); ++fi, ++refi)
  {
    u -= (*fi) * (*refi);
  }

  x = A * x + b*u;

  double zmp = c*x;
  err += (zmp - ref.front());
}

void ZMPPreviewController::control(Vector3d& com, Vector2d& dcom, Vector2d& ddcom) {
  ASSERT(ready());

  theZ = getZMPReferenceBuffer().front().z;
  setHeight(theZ);

  update(getZMPReferenceBuffer().getXRef(), theX, theErr.x);
  update(getZMPReferenceBuffer().getYRef(), theY, theErr.y);

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

bool ZMPPreviewController::ready() const {
  if (parameters.current == NULL) {
    return false;
  }

  Vector3<size_t> refZMPSize = getZMPReferenceBuffer().size();
  return refZMPSize.x >= previewSteps() && refZMPSize.y >= previewSteps();
}
