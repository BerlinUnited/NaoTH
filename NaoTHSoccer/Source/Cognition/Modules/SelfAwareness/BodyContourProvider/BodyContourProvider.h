/*
*@file BodyContourProvider.h
* Module declaration for providing the robot's contour in the image. 
* The Body Contour is used to exclude robot's limbs from image, so that they
* couldn't be confused with other objects
*
*@author Kirill Yasinovskiy
*/

#ifndef _BodyContourProvider_h_
#define _BodyContourProvider_h_

#include <ModuleFramework/Module.h>

//Tools
#include <Tools/NaoInfo.h>
#include "Tools/DoubleCamHelpers.h"
#include "Tools/Math/Geometry.h"
#include "Tools/ColorClasses.h"

//Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/DebugImageDrawings.h"

#include <vector>

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(BodyContourProvider)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings3D)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)

  REQUIRE(FrameInfo)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(KinematicChain)

  PROVIDE(BodyContour)
  PROVIDE(BodyContourTop)
END_DECLARE_MODULE(BodyContourProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

/**
* @class BodyContourProvider
* A module that provides the contour of the robot's body in the image.
*/

class BodyContourProvider : public BodyContourProviderBase
{
public:

  BodyContourProvider();
  virtual ~BodyContourProvider(){}

  virtual void execute()
  {
     execute(CameraInfo::Bottom);
     execute(CameraInfo::Top);
  }

  void execute(CameraInfo::CameraID id);

private:

  struct BodyParts
  {
    std::vector< Vector3d > torso, // the contour of the torso
                            upperArm, // the contour of the upper arm
                            lowerArm, // the contour of the lower arm
                            upperLeg, // the contour of the upper leg
                            lowerLeg, // the contour of the lower leg
                            foot; // the contour of the foot
  } bodyparts;

  /** 
  * add a line to BodyContour:
  * take the Position of a robot's limb (Pose3d) and multiply it
  * with the contour of this limb, finally, project the contour into the image
  */
  inline void add( const Pose3D& origin, const std::vector<Vector3d>& c, double sign, BodyContour::BodyPartID id);

  /**
  * updates the grid of the BodyContour representation based on the segment p1--p2
  */
  void updateBodyContour(const Vector2i& p1, const Vector2i& p2, BodyContour::BodyPartID id);

  void debug() const;
  void drawContur3D(const Pose3D& origin, const std::vector<Vector3d>& c, double sign, ColorClasses::Color color) const;


  inline bool isInsideImage(const Vector2i& p) const {
    return p.x >= 0 && p.x < (int)getCameraInfo().resolutionWidth && 
           p.y >= 0 && p.y < (int)getCameraInfo().resolutionHeight;
  }
   
  bool clampSegment(const Vector2i& ul, const Vector2i& lr, Vector2i& a, Vector2i& b) const;

  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(BodyContourProvider,CameraInfo);
  DOUBLE_CAM_REQUIRE(BodyContourProvider,CameraMatrix);

  DOUBLE_CAM_PROVIDE(BodyContourProvider,DebugImageDrawings);
  DOUBLE_CAM_PROVIDE(BodyContourProvider,BodyContour);

};// end class BodyContourProvider

#endif // _BodyContourProvider_h_
