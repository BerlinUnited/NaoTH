/**
* @file FootStepPlanner.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* plan the foot step according to motion request
*/

#ifndef _ZMP_PREVIEW_CONTROLLER_H
#define _ZMP_PREVIEW_CONTROLLER_H

#include <ModuleFramework/Module.h>

#include <list>
#include <vector>
#include <map>

#include "Tools/Math/Matrix3x3.h"
#include "Tools/Math/Vector2.h"

#include "Representations/Infrastructure/FrameInfo.h"

#include "Representations/Motion/Walk2018/ZMPReferenceBuffer.h"
#include "Representations/Motion/Walk2018/TargetCoMFeetPose.h"

#include "Representations/Motion/Walk2018/Walk2018Parameters.h"

#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugRequest.h"

BEGIN_DECLARE_MODULE(ZMPPreviewController)
    PROVIDE(DebugPlot)
    PROVIDE(DebugRequest)
    PROVIDE(DebugDrawings)

    REQUIRE(FrameInfo)

    PROVIDE(Walk2018Parameters) // changing height
    PROVIDE(ZMPReferenceBuffer) // poping
    PROVIDE(TargetCoMFeetPose)  // setting target CoM
END_DECLARE_MODULE(ZMPPreviewController)

class ZMPPreviewController : private ZMPPreviewControllerBase
{
  private:
    // precalculated values
    Matrix3x3<double> A;
    Vector3d b;
    Vector3d c;

  public:
    ZMPPreviewController();

    virtual void execute();

    // state of the controller

    void reset(){
      theErr = Vector2d(0, 0);
    }

    void control(Vector3d& com, Vector2d& dcom, Vector2d& ddcom);

    size_t previewSteps() const { return parameters.current->f.size(); }

    bool ready() const;

    unsigned int count() const { return static_cast<unsigned int> (getZMPReferenceBuffer().size().x); }

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

    bool is_stationary() const {
      return com_velocity().abs2() < parameters.stationary_threshold.velocity * parameters.stationary_threshold.velocity
             && com_acceleration().abs2() < parameters.stationary_threshold.acceleration * parameters.stationary_threshold.acceleration;
    }

    void init(const Vector3d& com)
    {
      ASSERT(previewSteps() > 1);
      // TODO: clear it because of the motion can be forced to finish immediately...
      // the idea of keep buffer is to switch zmp control between different motions,
      // such as walk and kick, then maybe we should check if zmp control is used every cycle and etc.

      setState(com, Vector2d(0,0), Vector2d(0,0));
    }

  private:
    void setHeight(double height);
    void update(const std::list<double>& ref, Vector3d&x, double& err) const;

    // output: internal state of the contsoller
    Vector3d theX; // x, x', x''
    Vector3d theY; // y, y', y''
    double theZ; // z
    Vector2d theErr;

    ZMPPreviewControllerParameter& parameters;
};

#endif // _ZMP_PLANNER_2018_H
