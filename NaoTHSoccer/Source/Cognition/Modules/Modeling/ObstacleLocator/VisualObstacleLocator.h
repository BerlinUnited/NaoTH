/**
 * @file VisualObstacleLocator.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Kirill Yasinovskiy</a>
 * Declaration of class RadarObstacleLocator
 */

#ifndef _VisualObstacleLocator_h_
#define _VisualObstacleLocator_h_

#include <ModuleFramework/Module.h>

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"

// tools 
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Math/Matrix2x2.h"

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/UltraSoundData.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/BodyState.h"
#include "Representations/Modeling/BallModel.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Modeling/RadarGrid.h"



//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(VisualObstacleLocator)
  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(OdometryData)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(CameraMatrix)

  REQUIRE(BallModel)
  REQUIRE(BallPercept)
  REQUIRE(UltraSoundReceiveData)
  REQUIRE(FieldInfo)
  

  PROVIDE(RadarGrid)
END_DECLARE_MODULE(VisualObstacleLocator)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class VisualObstacleLocator : private VisualObstacleLocatorBase
{
public:

  // default constructor
  VisualObstacleLocator();
  // default destructor
  virtual ~VisualObstacleLocator(){}

  // some variables

  // time after obstacle is forgotten
  enum {
    timeAfterWhichObstacleIsForgotten = 5000
  };
  // update mode
  enum UpdateMode{overwrite, extend, limit};

  virtual void execute();

private:

  OdometryData lastRobotOdometry;
  unsigned int lastTimeObstacleWasSeen;
};

#endif //_VisualObstacleLocator_h_
