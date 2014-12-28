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
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h" // just for debug
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/BodyContour.h"

// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/DataStructures/UniformGrid.h>

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

// Debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"

#include "Tools/DoubleCamHelpers.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SimpleFieldColorClassifier)
  PROVIDE(DebugPlot)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(StopwatchManager)
  
  REQUIRE(FrameInfo)
  REQUIRE(ColorChannelHistograms)
  REQUIRE(ColorChannelHistogramsTop)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)

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

    if(parameters.classifyInBothImages > 0)
    {
      execute(CameraInfo::Top);
    }

    cameraID = CameraInfo::Bottom;
    double lowBorderY = filteredHistogramY.median - parameters.filterFactorY * filteredHistogramY.sigma;
    double highBorderY = filteredHistogramY.median + parameters.filterFactorY * filteredHistogramY.sigma;

    DEBUG_REQUEST("Vision:ColorClassifiers:SimpleFieldColorClassifier:BottomCam:markYClassification",
      for(unsigned int x = 0; x < getImage().width(); x+=2) {
        for(unsigned int y = 0; y < getImage().height(); y+=2) {
          const Pixel& pixel = getImage().get(x, y);
          if(lowBorderY < pixel.y && pixel.y < highBorderY) {
            POINT_PX(ColorClasses::black, x, y);
          }
        }
      }
    );

    DEBUG_REQUEST("Vision:ColorClassifiers:SimpleFieldColorClassifier:BottomCam:mark_green",
      for(unsigned int x = 0; x < getImage().width(); x+=2) {
        for(unsigned int y = 0; y < getImage().height(); y+=2) {
          if(getFieldColorPercept().isFieldColor(getImage().get(x, y))) {
            POINT_PX(ColorClasses::green, x, y);
          }
        }
      } 
    );

    cameraID = CameraInfo::Top;
    DEBUG_REQUEST("Vision:ColorClassifiers:SimpleFieldColorClassifier:TopCam:markYClassification",
      for(unsigned int x = 0; x < getImage().width(); x+=2) {
        for(unsigned int y = 0; y < getImage().height(); y+=2) {
          const Pixel& pixel = getImage().get(x, y);
          if(lowBorderY < pixel.y && pixel.y < highBorderY) {
            POINT_PX(ColorClasses::black, x, y);
          }
        }
      }
    );

    DEBUG_REQUEST("Vision:ColorClassifiers:SimpleFieldColorClassifier:TopCam:mark_green",
      for(unsigned int x = 0; x < getImage().width(); x+=2) {
        for(unsigned int y = 0; y < getImage().height(); y+=2) {
          if(getFieldColorPercept().isFieldColor(getImage().get(x, y))) {
            POINT_PX(ColorClasses::green, x, y);
          }
        }
      } 
    );

  }

private:
  void execute(const CameraInfo::CameraID id);

  double gauss(double sigma, double mean, double x);

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("SimpleFieldColorClassifierParameters")
    {
      PARAMETER_REGISTER(fieldColorMax.u) = 16;
      PARAMETER_REGISTER(fieldColorMax.v) = 16;
      PARAMETER_REGISTER(filterFactorY) = 4;
      PARAMETER_REGISTER(classifyInBothImages) = 1;
      PARAMETER_REGISTER(collectionTimeSpan) = 5;

      syncWithConfig();
      //DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      //DebugParameterList::getInstance().remove(this);
    }

    DoublePixel fieldColorMax;
    DoublePixel fieldColorMin;
    double filterFactorY;
    int classifyInBothImages;
    int collectionTimeSpan;
  } parameters;

  inline Parameters& getParameters() {
    return parameters;
  }

  // id of the camera the module is curently running on
  CameraInfo::CameraID cameraID;

  UniformGrid uniformGrid; // subsampling of the image
  Statistics::Histogram<ColorChannelHistograms::VALUE_COUNT> filteredHistogramY;
  Statistics::Histogram<ColorChannelHistograms::VALUE_COUNT> filteredHistogramU;
  Statistics::Histogram<ColorChannelHistograms::VALUE_COUNT> filteredHistogramV;

  DOUBLE_CAM_PROVIDE(SimpleFieldColorClassifier, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(SimpleFieldColorClassifier, Image);
  DOUBLE_CAM_REQUIRE(SimpleFieldColorClassifier, ColorChannelHistograms);
  DOUBLE_CAM_REQUIRE(SimpleFieldColorClassifier, BodyContour);

  DOUBLE_CAM_PROVIDE(SimpleFieldColorClassifier,FieldColorPercept);
};

#endif  /* _SimpleFieldColorClassifier_H_ */
