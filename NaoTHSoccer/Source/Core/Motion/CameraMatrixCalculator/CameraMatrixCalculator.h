/**
* @file CameraMatrixCalculator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraMatrixCalculator
*/

#ifndef _CameraMatrixCalculator_h_
#define _CameraMatrixCalculator_h_

#include <Representations/Perception/CameraMatrix.h>
#include <Representations/Infrastructure/CameraInfo.h>

#include "Representations/Modeling/KinematicChain.h"

class CameraMatrixCalculator
{
private:

  CameraMatrixCalculator(){}; // don't create an instance

public:
  /** executes the module */
  static void calculateCameraMatrix(CameraMatrix& theCameraMatrix,
    const naoth::CameraInfo &theCameraInfo,
    const KinematicChain& theKinematicChain
  );
};

#endif //_CameraMatrixCalculator_h_
