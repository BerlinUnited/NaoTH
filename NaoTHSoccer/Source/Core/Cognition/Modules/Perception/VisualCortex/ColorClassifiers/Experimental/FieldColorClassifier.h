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
    setPercept();
  };

private:
  CameraInfo::CameraID cameraID;

  Statistics::Histogram<256> histY;
  Statistics::Histogram<256> histU;
  Statistics::Histogram<256> histV;

  Statistics::Histogram<256> histCommonY;
  Statistics::Histogram<256> histCommonV;
  Statistics::Histogram<256> histCommonU;

  Statistics::Histogram<256> histSigmaY;
  Statistics::Histogram<256> histSigmaV;
  Statistics::Histogram<256> histSigmaU;

  
  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("FieldColorParameters")
    {
      PARAMETER_REGISTER(deviationFactorY) = 3.0;
      PARAMETER_REGISTER(deviationFactorU) = 3.0;
      PARAMETER_REGISTER(deviationFactorV) = 3.0;
      PARAMETER_REGISTER(PostDeviationFactor) = 1.0;


      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    double deviationFactorY;
    double deviationFactorU;
    double deviationFactorV;
    double PostDeviationFactor;
  };

  Parameters params;

  void setPercept();
  double gauss(double sigma, double mean, double x);

  void runDebugRequests();



  DOUBLE_CAM_REQUIRE(FieldColorClassifier, Image);
  DOUBLE_CAM_REQUIRE(FieldColorClassifier, ColoredGrid);
  DOUBLE_CAM_REQUIRE(FieldColorClassifier, ColorChannelHistograms);
  
  DOUBLE_CAM_PROVIDE(FieldColorClassifier, FieldColorPercept);

};

#endif  /* FIELDCOLORCLASSIFIER_H */

