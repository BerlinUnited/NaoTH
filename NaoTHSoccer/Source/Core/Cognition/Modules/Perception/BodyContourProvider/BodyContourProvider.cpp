/*
*@file BodyContourProvider.h
* Module implementation for providing the robot's contour in the image. 
* The Body Contour is used to exclude robot's limbs from image, so that they
* couldn't be confused with other objects
*
*@author Kirill Yasinovskiy
*/

#include "BodyContourProvider.h"
#include "Tools/Math/Geometry.h"
#include "Tools/CameraGeometry.h"
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/Debug/DebugDrawings3D.h>
#include <Tools/Debug/DebugDrawings.h>

#include "Tools/ImageProcessing/BresenhamLineScan.h"

using namespace std;

BodyContourProvider::BodyContourProvider()
:
  cameraID(CameraInfo::Bottom)
{
  // init the grid
  getBodyContour().setGridSize(20, getCameraInfo().resolutionWidth, getCameraInfo().resolutionHeight);
  getBodyContourTop().setGridSize(20, getCameraInfo().resolutionWidth, getCameraInfo().resolutionHeight);

  // load contour parameters
  // torso
  bodyparts.torso.push_back(Vector3d(-60, 17, 80));
  bodyparts.torso.push_back(Vector3d(-53, 45, 80));
  bodyparts.torso.push_back(Vector3d( -35, 58, 100));
  bodyparts.torso.push_back(Vector3d( -32, 75, 115));
  bodyparts.torso.push_back(Vector3d(32, 75, 115));
  bodyparts.torso.push_back(Vector3d(35, 58, 100));
  bodyparts.torso.push_back(Vector3d(51, 45, 80));
  bodyparts.torso.push_back(Vector3d(59, 17, 80));
  bodyparts.torso.push_back(Vector3d(59, -17, 80));
  bodyparts.torso.push_back(Vector3d(51, -45, 80));
  bodyparts.torso.push_back(Vector3d(35, -58, 100));
  bodyparts.torso.push_back(Vector3d(32, -75, 115));
  bodyparts.torso.push_back(Vector3d(-32, -75, 115));
  bodyparts.torso.push_back(Vector3d(-35, -58, 100));
  bodyparts.torso.push_back(Vector3d(-53, -45, 80));
  bodyparts.torso.push_back(Vector3d(-60, -17, 80));   
  // upper arm
  bodyparts.upperArm.push_back(Vector3d(  15,  -5, -60));
  bodyparts.upperArm.push_back(Vector3d(  10,   5, -60));
  bodyparts.upperArm.push_back(Vector3d( -25,   5, -40));
  bodyparts.upperArm.push_back(Vector3d( -30,  25, -30));
  bodyparts.upperArm.push_back(Vector3d( -40,  25, -10));
  bodyparts.upperArm.push_back(Vector3d( -45,  20,   5));
  bodyparts.upperArm.push_back(Vector3d( -35,  10,  30));
  bodyparts.upperArm.push_back(Vector3d( -20, -10,  42));
  bodyparts.upperArm.push_back(Vector3d(   0, -22,  47));
  bodyparts.upperArm.push_back(Vector3d( 120, -22,  47));
  // lower arm
  bodyparts.lowerArm.push_back(Vector3d(-20, 30, 25));
  bodyparts.lowerArm.push_back(Vector3d(120, 30, 45));
  bodyparts.lowerArm.push_back(Vector3d(120, -40, 40));
  // upper leg
  bodyparts.upperLeg.push_back(Vector3d(50, 30, 0));
  bodyparts.upperLeg.push_back(Vector3d(50, 50, -115));
  bodyparts.upperLeg.push_back(Vector3d(-60, 45, -160));
  bodyparts.upperLeg.push_back(Vector3d(-60, -38, -160));
  bodyparts.upperLeg.push_back(Vector3d(50, -42, -115));
  bodyparts.upperLeg.push_back(Vector3d(50, -30, 0));
  // lower leg
  bodyparts.lowerLeg.push_back(Vector3d(50, 50, -115));
  bodyparts.lowerLeg.push_back(Vector3d(50, -42, -115));
  // foot
  bodyparts.foot.push_back(Vector3d(-20, 50, -35));
  bodyparts.foot.push_back(Vector3d(65, 50, -35));
  bodyparts.foot.push_back(Vector3d(105, 20, -35));
  bodyparts.foot.push_back(Vector3d(105, -20, -35));
  bodyparts.foot.push_back(Vector3d(80, -40, -35));
  bodyparts.foot.push_back(Vector3d(-20, -40, -35));
  
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d:feet", "draw 3D body contour of the feet", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d:legs", "draw 3D body contour of the legs", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d:torso", "draw 3D body contour of the torso", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d:shoulders", "draw 3D body contour of the shoulders", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_3d:forearms", "draw 3D body contour of the forearms", false);

  DEBUG_REQUEST_REGISTER("BodyContourProvider:draw_projected_contour_lines", "draw body contour on the image based on lines", false);

  DEBUG_REQUEST_REGISTER("BodyContourProvider:TopCam:draw_activated_cells", "draw activated cells", false);
  DEBUG_REQUEST_REGISTER("BodyContourProvider:BottomCam:draw_activated_cells", "draw activated cells", false);
}

void BodyContourProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS_PX(cameraID);

  getBodyContour().reset();


  // HACK: we do this because the kinematic chain may be inconsistent with the camera mtrix
  kinematicCameraMatrix = getKinematicChain().theLinks[KinematicChain::Head].M;
  kinematicCameraMatrix.conc(NaoInfo::robotDimensions.cameraTransformation[cameraID]);
 
  // apply the correction
  kinematicCameraMatrix.rotateY(getCameraMatrixOffset().correctionOffset[cameraID].y) // tilt
                       .rotateX(getCameraMatrixOffset().correctionOffset[cameraID].x); // roll
  // HACK: end

  // calculate body contours
  add(getKinematicChain().theLinks[KinematicChain::Torso].M, bodyparts.torso, 1, BodyContour::Torso);
  add(getKinematicChain().theLinks[KinematicChain::LThigh].M, bodyparts.upperLeg, 1, BodyContour::LegLeft);
  add(getKinematicChain().theLinks[KinematicChain::RThigh].M, bodyparts.upperLeg, -1, BodyContour::LegRight);
  add(getKinematicChain().theLinks[KinematicChain::LThigh].M, bodyparts.lowerLeg, 1, BodyContour::LegLeft);
  add(getKinematicChain().theLinks[KinematicChain::RThigh].M, bodyparts.lowerLeg, -1, BodyContour::LegRight);
  add(getKinematicChain().theLinks[KinematicChain::LFoot].M, bodyparts.foot, 1, BodyContour::FootLeft);
  add(getKinematicChain().theLinks[KinematicChain::RFoot].M, bodyparts.foot, -1, BodyContour::FootRight);
  add(getKinematicChain().theLinks[KinematicChain::LBicep].M, bodyparts.upperArm, 1, BodyContour::UpperArmLeft);
  add(getKinematicChain().theLinks[KinematicChain::RBicep].M, bodyparts.upperArm, -1, BodyContour::UpperArmRight);
  add(getKinematicChain().theLinks[KinematicChain::LForeArm].M, bodyparts.lowerArm, 1, BodyContour::LowerArmLeft);
  add(getKinematicChain().theLinks[KinematicChain::RForeArm].M, bodyparts.lowerArm, -1, BodyContour::LowerArmRight);

  // draw some debug stuff
  debug();
}// end execute


void BodyContourProvider::add(
  const Pose3D& origin, 
  const std::vector<Vector3d>& c, 
  double sign, 
  BodyContour::BodyPartID id)
{
  // constants
  const Vector2i frameUpperLeft(0,0);
  const Vector2i frameLowerRight(getCameraInfo().resolutionWidth-1, getCameraInfo().resolutionHeight-1);
  
  // estimate the position of contour-point based on current position of the limb
  Vector3d p1 = origin * Vector3d(c[0].x, c[0].y * sign, c[0].z);
  // project this point into the image
  Vector2i q1;
  bool q1_ok = CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), p1, q1);

  // start building the contour lines
  for(size_t i = 1; i < c.size(); i++)
  {
    Vector3d p2 = origin * Vector3d(c[i].x, c[i].y * sign, c[i].z);
    Vector2i q2;
    bool q2_ok = CameraGeometry::relativePointToImage(getCameraMatrix(), getCameraInfo(), p2, q2);

    // if the prjection was successful
    if (q1_ok && q2_ok)
    {
      Vector2i a(q1);
      Vector2i b(q2);
      
      bool lineCutsImage = clampSegment(frameUpperLeft, frameLowerRight, a, b);
      if(lineCutsImage) 
      {
        updateBodyContur(a,b,id);
      }

      // Treat the special cases when either q1 or q2 are above the image
      // NOTE: this is enough because we assume that a body part always covers
      //       the lower part of the image
      if(lineCutsImage)
      {
        if(q2.y < 0 && b.y == 0) {
          int x2 = Math::clamp(q2.x, frameUpperLeft.x, frameLowerRight.x);
          updateBodyContur(b, Vector2i(x2, 0), id);
        }

        if(q1.y < 0 && a.y == 0) {
          int x1 = Math::clamp(q1.x, frameUpperLeft.x, frameLowerRight.x);
          updateBodyContur(Vector2i(x1, 0), a, id);
        }
      }
      else
      {
        if(q1.y < 0 || q2.y < 0) {
          int x1 = Math::clamp(q1.x, frameUpperLeft.x, frameLowerRight.x);
          int x2 = Math::clamp(q2.x, frameUpperLeft.x, frameLowerRight.x);
          if(x1 != x2) {
            updateBodyContur(Vector2i(x1, 0), Vector2i(x2, 0), id);
          }
        }
      }
    }// end if projected

    p1 = p2;
    q1 = q2;
    q1_ok = q2_ok;
  }// end for
}// end add

void BodyContourProvider::updateBodyContur(const Vector2i& p1, const Vector2i& p2, BodyContour::BodyPartID id)
{
  const Vector2i cellOneCoords = getBodyContour().getCellCoord(p1);
  const Vector2i cellTwoCoords = getBodyContour().getCellCoord(p2);

  const BodyContour::Cell& cellOne = getBodyContour().getCell(cellOneCoords);
  const BodyContour::Cell& cellTwo = getBodyContour().getCell(cellTwoCoords);

  if(!cellOne.occupied || !cellTwo.occupied || cellOne.id == id || cellTwo.id == id)
  {
    DEBUG_REQUEST("BodyContourProvider:draw_projected_contour_lines",
      LINE_PX(ColorClasses::skyblue,p1.x, p1.y, p2.x, p2.y);
    );

    BresenhamLineScan scanLine(cellOneCoords, cellTwoCoords);
    Vector2i point(cellOneCoords);

    for(int i = 0; i <= scanLine.numberOfPixels; i++) {
      for (int j = point.y; j < getBodyContour().gridHeight(); j++) {
        getBodyContour().setCell(point.x, j, id, true);
      }
      scanLine.getNext(point);
    }
  }
}//end updateBodyContur

bool BodyContourProvider::clampSegment(const Vector2i& ul, const Vector2i& lr, Vector2i& a, Vector2i& b)
{
  bool aInside = isInsideImage(a);
  bool bInside = isInsideImage(b);

  if(!aInside || !bInside) {
    Math::LineSegment segment(a, b);
    Vector2i ac, bc;
    if(!Geometry::getIntersectionPointsOfLineAndRectangle(ul, lr, segment, ac, bc)) {
      return false;
    }

    // check if the clamped points are inside of the segment (a,b)
    if(!aInside && !bInside) {
      if(ac.x > max(a.x,b.x) || ac.x < min(a.x,b.x) || ac.y > max(a.y,b.y) || ac.y < min(a.y,b.y) ||
         bc.x > max(a.x,b.x) || bc.x < min(a.x,b.x) || bc.y > max(a.y,b.y) || bc.y < min(a.y,b.y))
      {
        return false;
      }
    }

    // clamp if necessary
    if(!aInside) { a = ac; }
    if(!bInside) { b = bc; }
  }

  return true;
}//end clampSegment


 void BodyContourProvider::debug()
 {
  DEBUG_REQUEST("BodyContourProvider:TopCam:draw_activated_cells",
    if(cameraID == CameraInfo::Top) {
      for(int i = 0; i < getBodyContour().gridWidth(); i++) {
        for (int j = 0; j < getBodyContour().gridHeight(); j++) {
          if (getBodyContour().getGrid()[i][j].occupied) {
              TOP_RECT_PX(ColorClasses::black, i*getBodyContour().cellSize(), j*getBodyContour().cellSize(), (i+1)*getBodyContour().cellSize(), (j+1)*getBodyContour().cellSize());
          }
        }
      }
    }
  );
  DEBUG_REQUEST("BodyContourProvider:BottomCam:draw_activated_cells",
    if(cameraID == CameraInfo::Bottom) {
      for(int i = 0; i < getBodyContour().gridWidth(); i++) {
        for (int j = 0; j < getBodyContour().gridHeight(); j++) {
          if (getBodyContour().getGrid()[i][j].occupied) {
              IMAGE_DRAWING_CONTEXT;
              RECT_PX(ColorClasses::black, i*getBodyContour().cellSize(), j*getBodyContour().cellSize(), (i+1)*getBodyContour().cellSize(), (j+1)*getBodyContour().cellSize());
          }
        }
      }
    }
  );

  DEBUG_REQUEST("BodyContourProvider:draw_3d:feet",
    drawContur3D(getKinematicChain().theLinks[KinematicChain::LFoot].M, bodyparts.foot,  1, ColorClasses::red);
    drawContur3D(getKinematicChain().theLinks[KinematicChain::RFoot].M, bodyparts.foot, -1, ColorClasses::red);
  );
  DEBUG_REQUEST("BodyContourProvider:draw_3d:legs",
    drawContur3D(getKinematicChain().theLinks[KinematicChain::LThigh].M, bodyparts.upperLeg,  1, ColorClasses::blue);
    drawContur3D(getKinematicChain().theLinks[KinematicChain::RThigh].M, bodyparts.upperLeg, -1, ColorClasses::blue);

    drawContur3D(getKinematicChain().theLinks[KinematicChain::LThigh].M, bodyparts.lowerLeg,  1, ColorClasses::blue);
    drawContur3D(getKinematicChain().theLinks[KinematicChain::RThigh].M, bodyparts.lowerLeg, -1, ColorClasses::blue);
  );
  DEBUG_REQUEST("BodyContourProvider:draw_3d:torso",
    drawContur3D(getKinematicChain().theLinks[KinematicChain::Torso].M, bodyparts.torso,  1, ColorClasses::green);
  );
  DEBUG_REQUEST("BodyContourProvider:draw_3d:shoulders",
    drawContur3D(getKinematicChain().theLinks[KinematicChain::LBicep].M, bodyparts.upperArm,  1, ColorClasses::yellow);
    drawContur3D(getKinematicChain().theLinks[KinematicChain::RBicep].M, bodyparts.upperArm, -1, ColorClasses::yellow);
  );
  DEBUG_REQUEST("BodyContourProvider:draw_3d:forearms",
    drawContur3D(getKinematicChain().theLinks[KinematicChain::LForeArm].M, bodyparts.lowerArm,  1, ColorClasses::orange);
    drawContur3D(getKinematicChain().theLinks[KinematicChain::RForeArm].M, bodyparts.lowerArm, -1, ColorClasses::orange);
  );
}//end debug

void BodyContourProvider::drawContur3D(
  const Pose3D& origin, 
  const std::vector<Vector3d>& c, 
  double sign,
  ColorClasses::Color color)
{
  Vector3d p1 = origin * Vector3d(c[0].x, c[0].y * sign, c[0].z);

  for(size_t i = 1; i < c.size(); i++)
  {
    Vector3d p2 = origin * Vector3d(c[i].x, c[i].y * sign, c[i].z);
    LINE_3D(color, p1, p2);
    p1 = p2;
  }
}