#ifndef __FieldAreaDetector_H_
#define __FieldAreaDetector_H_

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"

#include "Tools/Debug/DebugRequest.h"
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/Debug/DebugDrawings.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/Debug/DebugParameterList.h>
#include "Tools/Debug/DebugModify.h"

#include "Tools/DoubleCamHelpers.h"

BEGIN_DECLARE_MODULE(FieldAreaDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugParameterList)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)

  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)
END_DECLARE_MODULE(FieldAreaDetector)


class FieldAreaDetector : private FieldAreaDetectorBase
{
public:


  FieldAreaDetector();
  virtual ~FieldAreaDetector();

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

  void execute(CameraInfo::CameraID id);

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("FieldAreaDetector")
    {
      PARAMETER_REGISTER(proportion_of_green) = 0.7;
      syncWithConfig();
    }
    double proportion_of_green;
  } params;


private:
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_PROVIDE(FieldAreaDetector, DebugImageDrawings);

  // double cam interface
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, Image);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(FieldAreaDetector, BodyContour);

  DOUBLE_CAM_REQUIRE(FieldAreaDetector, BallDetectorIntegralImage);
};

#endif  /* __FieldAreaDetector_H_ */

