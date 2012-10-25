/**
* @file CameraMatrixCorrector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraMatrixProvider
*/

#ifndef _CameraMatrixCorrector_h_
#define _CameraMatrixCorrector_h_

#include <ModuleFramework/Module.h>

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/GoalPercept.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/AccelerometerData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/InertialModel.h"
#include "Representations/Modeling/CameraMatrixOffset.h"

// motion stuff
#include "Representations/Modeling/KinematicChain.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CameraMatrixCorrector)
  REQUIRE(InertialModel)
  REQUIRE(GoalPercept) // needed fot calibration of the camera matrix
  REQUIRE(Image)
  REQUIRE(FieldInfo)
  REQUIRE(AccelerometerData)
  REQUIRE(FrameInfo)
  REQUIRE(CameraMatrixOffset)
  REQUIRE(CameraMatrix)

  PROVIDE(SensorJointData)
  PROVIDE(KinematicChain)
END_DECLARE_MODULE(CameraMatrixCorrector)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class CameraMatrixCorrector: public CameraMatrixCorrectorBase
{
public:

  CameraMatrixCorrector();
  ~CameraMatrixCorrector();


  /** executes the module */
  void execute();

private:
  unsigned int udpateTime;
  Vector3<double> theFSRPos[FSRData::numOfFSR];

  void calibrate();
  void reset_calibration();
  double projectionError(double offsetX, double offsetY);
};

#endif //_CameraMatrixCorrector_h_
