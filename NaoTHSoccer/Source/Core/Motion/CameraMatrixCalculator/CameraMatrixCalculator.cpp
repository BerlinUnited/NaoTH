/**
* @file CameraMatrixCalculator.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class CameraMatrixCalculator
*/

#include "CameraMatrixCalculator.h"
#include "PlatformInterface/Platform.h"

void CameraMatrixCalculator::calculateCameraMatrix(
    CameraMatrix& theCameraMatrix,
    CameraInfo::CameraID cameraId,
    const KinematicChain& theKinematicChain
  )
{
  const CameraInfoParameter& theCameraInfo = Platform::getInstance().theCameraInfo;

  // get the pose of the head
  Pose3D pose(theKinematicChain.theLinks[KinematicChain::Head].M);

  // apply the correction
  pose.rotateY(theCameraInfo.cameraTiltOffset)
      .rotateX(theCameraInfo.cameraRollOffset);

  // transformation from the head to the camera
  pose.conc(theCameraInfo.transformation[cameraId]);
  
  // copy the result
  theCameraMatrix = pose;

  theCameraMatrix.cameraNumber = (int)cameraId;
}//end execute

