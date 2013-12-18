/**
* @file CameraMatrixCalculator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraMatrixCalculator
*/

#ifndef _CameraMatrixCalculator_h_
#define _CameraMatrixCalculator_h_

#include <Representations/Infrastructure/CameraInfo.h>
#include <Representations/Perception/CameraMatrix.h>
#include <Representations/Modeling/CameraMatrixOffset.h>
#include <Representations/Modeling/KinematicChain.h>

class CameraMatrixCalculator
{
private:
  CameraMatrixCalculator(){}; // don't create an instance

public:
  static void calculateCameraMatrix(
    CameraMatrix& theCameraMatrix,
    const KinematicChain& theKinematicChain,
    const Pose3D& theCameraOffset,
    const Vector2d& theCameraCorrectionOffset
  );
};

#endif //_CameraMatrixCalculator_h_
