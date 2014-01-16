/**
* @file CameraMatrixCalculator.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraMatrixCalculator
*/

#include "CameraMatrixCalculator.h"
#include <Tools/NaoInfo.h>

void CameraMatrixCalculator::calculateCameraMatrix(
    CameraMatrix& theCameraMatrix,
    const KinematicChain& theKinematicChain,
    const Pose3D& theCameraOffset,
    const Vector2d& theCameraCorrectionOffset
  )
{
  // get the pose of the head
  Pose3D pose(theKinematicChain.theLinks[KinematicChain::Head].M);

  // transformation from the head to the camera
  pose.conc(theCameraOffset);
  
  // apply the correction
  pose.rotateY(theCameraCorrectionOffset.y) // tilt
      .rotateX(theCameraCorrectionOffset.x); // roll

  // copy the result
  theCameraMatrix = pose;

  //theCameraMatrix.cameraID = id;
}//end execute

