/* 
 * File:   FieldDetector.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef __FieldDetector_H_
#define __FieldDetector_H_

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"

#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugImageDrawings.h>

#include "Tools/DoubleCamHelpers.h"

BEGIN_DECLARE_MODULE(FieldDetector)
  PROVIDE(DebugRequest)  
  PROVIDE(DebugImageDrawings)  
  PROVIDE(DebugImageDrawingsTop)  
  
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  PROVIDE(FieldPercept)
  PROVIDE(FieldPerceptTop)
END_DECLARE_MODULE(FieldDetector)


class FieldDetector : private FieldDetectorBase
{
public:
  FieldDetector();
  virtual ~FieldDetector();

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

  void execute(CameraInfo::CameraID id);

private:
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_PROVIDE(FieldDetector, DebugImageDrawings);

  // double cam interface
  DOUBLE_CAM_REQUIRE(FieldDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(FieldDetector, Image);
  DOUBLE_CAM_REQUIRE(FieldDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(FieldDetector, ScanLineEdgelPercept);

  DOUBLE_CAM_PROVIDE(FieldDetector, FieldPercept);
};

#endif  /* __FieldDetector_H_ */

