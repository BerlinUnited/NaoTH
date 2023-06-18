/* 
 * File:   FieldColorClassifier.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef FieldColorClassifier_H
#define FieldColorClassifier_H

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h" // just for debug
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/ColorTable64.h"

// Tools
#include <Tools/Math/Vector2.h>
#include <Tools/DataStructures/UniformGrid.h>
#include <Tools/DataStructures/Histogram.h>

// Debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Tools/DoubleCamHelpers.h"

#include "Tools/Math/Moments2.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(FieldColorClassifier)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugPlot)
  PROVIDE(DebugModify)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)
  
  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)

  PROVIDE(FieldColorPercept)
  PROVIDE(FieldColorPerceptTop)

  PROVIDE(ColorTable64)
END_DECLARE_MODULE(FieldColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class FieldColorClassifier : public  FieldColorClassifierBase
{
private:
  class Parameters: public ParameterList
  {
  public:
    Parameters(std::string name = "FieldColorClassifier") : ParameterList(name)
    {
      PARAMETER_REGISTER(green.brightnesConeOffset) = 30;
      PARAMETER_REGISTER(green.brightnesConeRadiusBlack) = 2;
      PARAMETER_REGISTER(green.brightnesConeRadiusWhite) = 50;

      PARAMETER_REGISTER(green.colorAngleCenter) = -2.4;
      PARAMETER_REGISTER(green.colorAngleWith) = 0.8;


      PARAMETER_REGISTER(red.brightnesConeOffset) = 16;
      PARAMETER_REGISTER(red.brightnesConeRadiusBlack) = 10;
      PARAMETER_REGISTER(red.brightnesConeRadiusWhite) = 70;

      PARAMETER_REGISTER(red.colorAngleCenter) = -4.4;
      PARAMETER_REGISTER(red.colorAngleWith) = 0.4;

      PARAMETER_REGISTER(provide_colortable) = false;

      syncWithConfig();
    }

    virtual ~Parameters() {}

    FieldColorPercept::HSISeparatorOptimized::Parameter green;
    FieldColorPercept::HSISeparatorOptimized::Parameter red;

    bool provide_colortable;
  };
public:
  FieldColorClassifier();
  virtual ~FieldColorClassifier();

  void execute()
  {
    if(paramsBottom.check_changed() || paramsTop.check_changed()) {
      frameWhenParameterChanged = getFrameInfo();
    }
    execute(CameraInfo::Top, paramsTop);
    execute(CameraInfo::Bottom, paramsBottom);
  }

private:
  void execute(const CameraInfo::CameraID id, Parameters& parameters);
  void updateCache();
  void debug(Parameters& parameters);

  

  class ParametersBottom: public Parameters
  {
  public:
    ParametersBottom() : Parameters("FieldColorClassifier") {}
    virtual ~ParametersBottom() {}
  } paramsBottom;

  class ParametersTop: public Parameters
  {
  public:
    ParametersTop() :  Parameters("FieldColorClassifierTop") {}
    virtual ~ParametersTop() {}
  } paramsTop;
  
  naoth::FrameInfo frameWhenParameterChanged;

private: // doublecam

    // id of the camera the module is curently running on
    CameraInfo::CameraID cameraID;

    DOUBLE_CAM_PROVIDE(FieldColorClassifier, DebugImageDrawings);

    DOUBLE_CAM_REQUIRE(FieldColorClassifier, Image);
    DOUBLE_CAM_REQUIRE(FieldColorClassifier, BodyContour);
    DOUBLE_CAM_REQUIRE(FieldColorClassifier, ArtificialHorizon);
    DOUBLE_CAM_PROVIDE(FieldColorClassifier, FieldColorPercept);

private:
  UniformGrid uniformGrid; // subsampling of the image

  class Histogram2D
  {
  private:
    std::vector<std::vector<double> > data;
  public:
    void setSize(size_t size) {
      data.resize(size);

      for(size_t i = 0; i < data.size(); i++) {
        data[i].resize(size);
      }
    }

    void clear() {
      for(size_t i = 0; i < data.size(); i++) {
        for(size_t j = 0; j < data[i].size(); j++) {
          data[i][j] = 0.0;
        }
      }
    }

    double& operator() (size_t x, size_t y) {
      return data[x][y];
    }

    double operator() (size_t x, size_t y) const {
      return data[x][y];
    }

    size_t size() const {
      return data.size();
    }
  };

  Histogram2D histogramYCromaArray[CameraInfo::numOfCamera];
  Histogram2D histogramUVArray[CameraInfo::numOfCamera];
  Histogram2D histogramUVBallArray[CameraInfo::numOfCamera];

private: // debug
  void plot(std::string id, Statistics::HistogramX& histogram) const
  {
    for(unsigned int i = 0; i < histogram.size; i++) {
      PLOT_GENERIC(id + ":rawHistogram" + ((cameraID == CameraInfo::Top)?"Top":"Bottom"), i, histogram.rawData[i]);
    }
  }

  void draw_UVSeparator(double angleCenter, double angleWidth) const;
  void draw_YChromaSeparator(double brightnesConeOffset, double brightnesConeRadiusBlack, double brightnesConeRadiusWhite) const;
  void draw_histogramUV(const Histogram2D& histUV) const;

};

#endif  /* FieldColorClassifier_H */
