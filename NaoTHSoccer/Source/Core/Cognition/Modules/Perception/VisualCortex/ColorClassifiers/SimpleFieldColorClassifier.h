/* 
 * File:   SimpleFieldColorClassifier.h
 * Author: claas
 * Author: Heinrich Mellmann
 *
 * Created on 15. März 2011, 15:56
 */

#ifndef _SimpleFieldColorClassifier_H_
#define _SimpleFieldColorClassifier_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Tools/ImageProcessing/Histograms.h"
#include "Representations/Infrastructure/Image.h" // just for debug

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/ImageProcessing/Grid/uniformGrid.h>

#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SimpleFieldColorClassifier)
  REQUIRE(FrameInfo)
  REQUIRE(ColorChanelHistograms)
  REQUIRE(ColorChanelHistogramsTop)
  REQUIRE(Image)
  REQUIRE(ImageTop)

  PROVIDE(FieldColorPercept)
  PROVIDE(FieldColorPerceptTop)
END_DECLARE_MODULE(SimpleFieldColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class SimpleFieldColorClassifier : public  SimpleFieldColorClassifierBase
{
public:
  SimpleFieldColorClassifier();
  virtual ~SimpleFieldColorClassifier(){}

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

private:
  void execute(const CameraInfo::CameraID id);


  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("SimpleFieldColorClassifierParameters")
    {
      PARAMETER_REGISTER(fieldColorMax.y) = 64;
      PARAMETER_REGISTER(fieldColorMin.y) = 64;
      PARAMETER_REGISTER(fieldColorMax.u) = 8;
      PARAMETER_REGISTER(fieldColorMax.v) = 10;

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    DoublePixel fieldColorMax;
    DoublePixel fieldColorMin;
  } parameters;

  inline Parameters& getParameters() {
    return parameters;
  }

  UniformGrid uniformGrid; // subsampling of the image
  Statistics::Histogram<ColorChanelHistograms::VALUE_COUNT> filteredHistogramY;
  Statistics::Histogram<ColorChanelHistograms::VALUE_COUNT> filteredHistogramU;

  // id of the camera the module is curently running on
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_REQUIRE(SimpleFieldColorClassifier,Image);
  DOUBLE_CAM_REQUIRE(SimpleFieldColorClassifier,ColorChanelHistograms);
  DOUBLE_CAM_PROVIDE(SimpleFieldColorClassifier,FieldColorPercept);
};

#endif  /* _SimpleFieldColorClassifier_H_ */