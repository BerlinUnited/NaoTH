#ifndef _ScanGridProvider_H_
#define _ScanGridProvider_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/ScanGrid.h"

// Tools
#include "Tools/DoubleCamHelpers.h"
#include "Tools/CameraGeometry.h"
#include <Tools/Math/Vector2.h>

// debug
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(ScanGridProvider)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  PROVIDE(ScanGrid)
  PROVIDE(ScanGridTop)
END_DECLARE_MODULE(ScanGridProvider)


class ScanGridProvider : private ScanGridProviderBase
{
public:
  ScanGridProvider();
  virtual ~ScanGridProvider();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

  double calculateMaxFieldWidth(int y);

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("ScanGridProvider")
    {
      PARAMETER_REGISTER(max_scan_distance_mm) = 9700;
      PARAMETER_REGISTER(max_vertical_scanlines) = 320;
      PARAMETER_REGISTER(vertical_gap_mm) = 100/2;
      PARAMETER_REGISTER(horizontal_gap_mm) = 50/2;
      PARAMETER_REGISTER(min_horizontal_gap_px) = 2;

      syncWithConfig();
    }

    double max_scan_distance_mm;
    int max_vertical_scanlines;
    double vertical_gap_mm;
    double horizontal_gap_mm;
    double min_horizontal_gap_px;
  } parameters;

private:
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_PROVIDE(ScanGridProvider, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(ScanGridProvider, Image);
  DOUBLE_CAM_REQUIRE(ScanGridProvider, CameraInfo);
  DOUBLE_CAM_REQUIRE(ScanGridProvider, CameraMatrix);
  DOUBLE_CAM_REQUIRE(ScanGridProvider, ArtificialHorizon);

  DOUBLE_CAM_PROVIDE(ScanGridProvider, ScanGrid);
};

#endif  /* _ScanGridProvider_H_ */
