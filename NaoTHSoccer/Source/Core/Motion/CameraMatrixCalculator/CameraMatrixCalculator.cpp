/**
* @file CameraMatrixCalculator.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraMatrixCalculator
*/

#include "CameraMatrixCalculator.h"

void CameraMatrixCalculator::calculateCameraMatrix(
    CameraMatrix& theCameraMatrix,
    const naoth::CameraInfo& theCameraInfo,
    const KinematicChain& theKinematicChain
  )
{
  // get the pose of the head
  Pose3D pose(theKinematicChain.theLinks[KinematicChain::Head].M);

  // transformation from the head to the camera
  pose.conc(theCameraInfo.transformation[theCameraInfo.cameraID]);
  
  // apply the correction
  pose.rotateY(theCameraInfo.correctionOffset[theCameraInfo.cameraID].y) // tilt
      .rotateX(theCameraInfo.correctionOffset[theCameraInfo.cameraID].x); // roll

  // copy the result
  theCameraMatrix = pose;

  theCameraMatrix.cameraID = theCameraInfo.cameraID;
}//end execute

