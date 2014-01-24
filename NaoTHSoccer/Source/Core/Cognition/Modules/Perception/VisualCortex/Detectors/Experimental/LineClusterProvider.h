/* 
 * File:   LineClusterProviderDifferential.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _LineClusterProviderDifferential_H_
#define _LineClusterProviderDifferential_H_

#include <ModuleFramework/Module.h>

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/ImageProcessing/ImageDrawings.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/ImageProcessing/Edgel.h"


// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Modeling/ColorClassificationModel.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Perception/BodyContour.h"


#include <Tools/DataStructures/ParameterList.h>


BEGIN_DECLARE_MODULE(LineClusterProvider)
  REQUIRE(Image)
  REQUIRE(ColorClassificationModel)
  //REQUIRE(CurrentCameraSettings)
  REQUIRE(CameraMatrix)
  REQUIRE(FieldPercept)
  REQUIRE(BodyContour)

  REQUIRE(ScanLineEdgelPercept)
END_DECLARE_MODULE(LineClusterProvider)


class LineClusterProvider : private LineClusterProviderBase
{
public:
  LineClusterProvider();
  virtual ~LineClusterProvider();


  void execute();


  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("LineClusterProvider")
    {
      PARAMETER_REGISTER(brightness_threshold) = 6;
      PARAMETER_REGISTER(scanline_count) = 23;
      PARAMETER_REGISTER(pixel_border_y) = 3;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    // threshold for detection of the jumps in the Y channel
    int brightness_threshold;

    // number of scanlines
    int scanline_count;

    // don't scan the lower lines in the image
    int pixel_border_y;
  } theParameters;



};

#endif  /* _LineClusterProvider_H_ */

