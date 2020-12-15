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
#include "Representations/Perception/BodyContour.h"

#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>

#include "Tools/DoubleCamHelpers.h"

BEGIN_DECLARE_MODULE(FieldDetector)
  PROVIDE(DebugRequest)  
  PROVIDE(DebugImageDrawings)  
  PROVIDE(DebugImageDrawingsTop)  
  PROVIDE(DebugParameterList)
  
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)

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

  static bool myVecCompareX(const Vector2i &first, const Vector2i &second);
  
  class Parameters: public ParameterList
  {
  public:
	
    Parameters() : ParameterList("FieldDetectorParameters")
    {
      PARAMETER_REGISTER(pruneThresholdArea) = 0.99;
      syncWithConfig();
    }
    double pruneThresholdArea;
  } params;

private:


  struct cmpVectorX {
    bool operator()(const Vector2i &first, const Vector2i &second) const {

    return (first.x<second.x);
    }
  } cmpVectorInstance;

private:
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_PROVIDE(FieldDetector, DebugImageDrawings);

  // double cam interface
  DOUBLE_CAM_REQUIRE(FieldDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(FieldDetector, Image);
  DOUBLE_CAM_REQUIRE(FieldDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(FieldDetector, ScanLineEdgelPercept);
  DOUBLE_CAM_REQUIRE(FieldDetector, BodyContour);

  DOUBLE_CAM_PROVIDE(FieldDetector, FieldPercept);
};

#endif  /* __FieldDetector_H_ */

