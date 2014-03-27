/* 
 * File:   FieldColorClassifier.h
 * Author: claas
 *
 * Created on 15. march 2011, 15:56
 */

#ifndef FIELDCOLORCLASSIFIER_H
#define FIELDCOLORCLASSIFIER_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColoredGrid.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/FieldColorPercept.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldColorClassifier)
  REQUIRE(ColorChannelHistograms)
  REQUIRE(ColorChannelHistogramsTop)
  REQUIRE(ColoredGrid)
  REQUIRE(ColoredGridTop)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(FrameInfo)

  PROVIDE(FieldColorPercept)
  PROVIDE(FieldColorPerceptTop)
END_DECLARE_MODULE(FieldColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class FieldColorClassifier : public  FieldColorClassifierBase
{
public:
  FieldColorClassifier();
  virtual ~FieldColorClassifier(){}

  // override the Module execute method
  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
     execute(CameraInfo::Top);
     execute(CameraInfo::Bottom);
  };

private:
  CameraInfo::CameraID cameraID;

  Statistics::Histogram<256> histY_1;
  Statistics::Histogram<256> histY;
  Statistics::Histogram<256> histU;
  Statistics::Histogram<256> histV_1;
  Statistics::Histogram<256> histV_2;
  
  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("FieldColorParameters")
    {
      PARAMETER_REGISTER(deviationFactor) = 2.0;
      PARAMETER_REGISTER(fV1) = 2.0;
      PARAMETER_REGISTER(fV2) = 1.0;
      PARAMETER_REGISTER(fU) = 1.0;
      PARAMETER_REGISTER(fY) = 1.0;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    double deviationFactor;
    double fV1;
    double fV2;
    double fU;
    double fY;
  };

  Parameters params;


  double gauss(double sigma, double mean, double x);

  void runDebugRequests();



  DOUBLE_CAM_REQUIRE(FieldColorClassifier, Image);
  DOUBLE_CAM_REQUIRE(FieldColorClassifier, ColoredGrid);
  DOUBLE_CAM_REQUIRE(FieldColorClassifier, ColorChannelHistograms);
  
  DOUBLE_CAM_PROVIDE(FieldColorClassifier, FieldColorPercept);

};

#endif  /* FIELDCOLORCLASSIFIER_H */

