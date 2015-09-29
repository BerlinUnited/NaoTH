/* 
 * File:   FieldColorClassifier.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _FieldColorClassifier_H_
#define _FieldColorClassifier_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Image.h" // just for debug
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BodyContour.h"

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
END_DECLARE_MODULE(FieldColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class FieldColorClassifier : public  FieldColorClassifierBase
{
public:
  FieldColorClassifier();
  virtual ~FieldColorClassifier();

  void execute()
  {
    // set the percept
    getFieldColorPercept().greenHSISeparator.setBrightness(parameters.brightnesConeOffset,parameters.brightnesConeRadiusWhite, parameters.brightnesConeRadiusBlack);
    getFieldColorPercept().greenHSISeparator.setColor(parameters.greenColorAngleCenter, parameters.greenColorAngleWith);

    // run debug stuff
    DEBUG_REQUEST("Vision:FieldColorClassifier:CamBottom", execute(CameraInfo::Bottom); );
    DEBUG_REQUEST("Vision:FieldColorClassifier:CamTop", execute(CameraInfo::Top); );
  }

private:
  void execute(const CameraInfo::CameraID id);

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("FieldColorClassifier")
    {
      PARAMETER_REGISTER(brightnesConeOffset) = 16;
      PARAMETER_REGISTER(brightnesConeRadiusWhite) = 16;
      PARAMETER_REGISTER(brightnesConeRadiusBlack) = 2.0;

      PARAMETER_REGISTER(greenColorAngleCenter) = 4.0;
      PARAMETER_REGISTER(greenColorAngleWith) = 1;

      syncWithConfig();
    }

    ~Parameters() {}

    int brightnesConeOffset;
    double brightnesConeRadiusWhite;
    double brightnesConeRadiusBlack;

    double greenColorAngleCenter;
    double greenColorAngleWith;
  } parameters;


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

    double& operator() (int x, int y) {
      return data[x][y];
    }

    double operator() (int x, int y) const {
      return data[x][y];
    }

    size_t size() const {
      return data.size();
    }
  };

  Histogram2D histogramYCromaArray[CameraInfo::numOfCamera];
  Histogram2D histogramUVArray[CameraInfo::numOfCamera];

private: // debug
  void plot(std::string id, Statistics::HistogramX& histogram) const
  {
    for(int i = 0; i < histogram.size; i++) {
      PLOT_GENERIC(id + ":rawHistogram" + ((cameraID == CameraInfo::Top)?"Top":"Bottom"), i, histogram.rawData[i]);
    }
  }

  void draw_histogramUV(const Histogram2D& histUV) const;

private: // doublecam
  
  // id of the camera the module is curently running on
  CameraInfo::CameraID cameraID;
  
  DOUBLE_CAM_PROVIDE(FieldColorClassifier, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(FieldColorClassifier, Image);
  DOUBLE_CAM_REQUIRE(FieldColorClassifier, BodyContour);
  DOUBLE_CAM_REQUIRE(FieldColorClassifier, ArtificialHorizon);
  DOUBLE_CAM_PROVIDE(FieldColorClassifier, FieldColorPercept);
};

#endif  /* _FieldColorClassifier_H_ */
