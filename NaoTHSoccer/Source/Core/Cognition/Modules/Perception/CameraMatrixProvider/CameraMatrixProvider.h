/**
* @file CameraMatrixProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class CameraMatrixProvider
*/

#ifndef _CameraMatrixProvider_h_
#define _CameraMatrixProvider_h_

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

// motion stuff
#include "Representations/Modeling/KinematicChain.h"


//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(CameraMatrixProvider)
  REQUIRE(InertialModel)
  REQUIRE(GoalPercept) // needed fot calibration of the camera matrix
  REQUIRE(Image)
  REQUIRE(FieldInfo)
  REQUIRE(AccelerometerData)
  REQUIRE(FrameInfo)

  PROVIDE(SensorJointData)
  PROVIDE(KinematicChain)
  PROVIDE(CameraMatrix)
END_DECLARE_MODULE(CameraMatrixProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class CameraMatrixProvider: public CameraMatrixProviderBase
{
public:

  CameraMatrixProvider();
  ~CameraMatrixProvider();


  /** executes the module */
  void execute();

private:
  unsigned int udpateTime;
  Vector3<double> theFSRPos[FSRData::numOfFSR];

  void calibrate();
  void reset_calibration();
  double projectionError(double offsetX, double offsetY);
};

#endif //__CameraMatrixProvider_h_
