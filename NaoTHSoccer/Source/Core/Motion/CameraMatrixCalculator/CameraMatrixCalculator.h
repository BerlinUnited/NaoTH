/**
* @file CameraMatrixCalculator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraMatrixCalculator
*/

#ifndef __CameraMatrixCalculator_h_
#define __CameraMatrixCalculator_h_

#include <Representations/Perception/CameraMatrix.h>
#include <Representations/Infrastructure/CameraInfo.h>

#include "Representations/Modeling/KinematicChain.h"

class CameraMatrixCalculator
{
private:

  CameraMatrixCalculator(){}; // don't create an instance

public:
  /** executes the module */
  static void calculateCameraMatrix(
    CameraMatrix& theCameraMatrix,
    CameraInfo::CameraID cameraId,
    const KinematicChain& theKinematicChain
  );
};

#endif //__CameraMatrixCalculator_h_
