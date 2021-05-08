#ifndef _MultiPassBallDetector_H_
#define _MultiPassBallDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>

#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/BodyContour.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Modeling/BallModel.h"

#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Perception/BallCandidates.h"
#include "Representations/Perception/MultiBallPercept.h"

// tools
#include "Tools/DoubleCamHelpers.h"

// local tools
#include "Tools/BestPatchList.h"
#include "Tools/BallKeyPointExtractor.h"
#include "Tools/BlackSpotExtractor.h"
#include "Tools/DataStructures/RingBufferWithSum.h"

#include "Classifier/AbstractCNNClassifier.h"

// debug
#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugPlot.h"

#include <memory>

BEGIN_DECLARE_MODULE(MultiPassBallDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)
  PROVIDE(DebugModify)
  PROVIDE(DebugPlot)
  PROVIDE(StopwatchManager)

  REQUIRE(FrameInfo)

  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)

  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(FieldInfo)
  REQUIRE(BallModel)

  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)

  PROVIDE(BallCandidates)
  PROVIDE(BallCandidatesTop)
  PROVIDE(MultiBallPercept)
END_DECLARE_MODULE(MultiPassBallDetector)


class MultiPassBallDetector: private MultiPassBallDetectorBase, private ModuleManager
{
public:
  MultiPassBallDetector();
  ~MultiPassBallDetector();

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    // update selected classifier from parameters
    if(params.check_changed()) {
      setClassifier(params.classifier, params.classifierClose);
    }

    getMultiBallPercept().reset();

    stopwatch_values.clear();

    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);


    // debug stuff
    double mean = 0;
    if(!stopwatch_values.empty()){
        for (auto i = stopwatch_values.begin(); i < stopwatch_values.end(); ++i){
            mean += *i;
        }
        mean /= static_cast<double>(stopwatch_values.size());
    }
    mean_of_means.add(mean);
    double average_mean = mean_of_means.getAverage();

    PLOT("MultiPassBallDetector:mean",mean);
    PLOT("MultiPassBallDetector:mean_of_means",average_mean);
  }

private:
  struct Parameters: public ParameterList
  {
    Parameters() : ParameterList("MultiPassBallDetector")
    {
      PARAMETER_REGISTER(keyDetector.borderRadiusFactorClose) = 0.5;
      PARAMETER_REGISTER(keyDetector.borderRadiusFactorFar) = 0.8;
      PARAMETER_REGISTER(keyDetector.maxInnerGreenDensitiy) = 0.5;
      
      PARAMETER_REGISTER(cnn.threshold) = 0.4;
      PARAMETER_REGISTER(cnn.thresholdClose) = 0.45;
      // Constant offset added to the input of the CNN. < 0 darker, > 0 brighter. T
      PARAMETER_REGISTER(cnn.meanBrightnessOffset) = 0.0; 
      

      PARAMETER_REGISTER(maxNumberOfKeys) = 12;
      PARAMETER_REGISTER(numberOfExportBestPatches) = 2;
      PARAMETER_REGISTER(providePatches) = false;

      PARAMETER_REGISTER(postBorderFactorClose) = 0.0;
      PARAMETER_REGISTER(postBorderFactorFar) = 0.3;
      PARAMETER_REGISTER(postMaxCloseSize) = 60;

      PARAMETER_REGISTER(checkContrast) = true;
      PARAMETER_REGISTER(contrastMinimum) = 50;
      PARAMETER_REGISTER(contrastMinimumClose) = 50;


      PARAMETER_REGISTER(classifier) = "fy1500_conf";
      PARAMETER_REGISTER(classifierClose) = "fy1500_conf";

      PARAMETER_REGISTER(brightnessMultiplierBottom) = 1.0;
      PARAMETER_REGISTER(brightnessMultiplierTop) = 1.0;
      
      
      syncWithConfig();
    }

    BallKeyPointExtractor::Parameter keyDetector;

    struct CNN {
      double threshold;
      double thresholdClose;

      double meanBrightnessOffset;
    } cnn;

    int maxNumberOfKeys;
    int numberOfExportBestPatches;
    bool providePatches;

    double postBorderFactorClose;
    double postBorderFactorFar;
    int postMaxCloseSize;

    bool checkContrast;
    double contrastMinimum;
    double contrastMinimumClose;

    std::string classifier;
    std::string classifierClose;

    double brightnessMultiplierBottom;
    double brightnessMultiplierTop;

  } params;

private:

  std::shared_ptr<AbstractCNNFinder> currentCNN;
  std::string currentCNNName;

  std::shared_ptr<AbstractCNNFinder> currentCNNClose;
  std::string currentCNNCloseName;

  std::map<std::string, std::shared_ptr<AbstractCNNFinder> > cnnMap;

  ModuleCreator<BallKeyPointExtractor>* theBallKeyPointExtractor;
  BestPatchList best;

private:
  void calculateCandidates();
  void addBallPercept(const Vector2d& center, double radius);
  void extractPatches();
  void providePatches();
  void addPatchByLastBall();

  void setClassifier(const std::string& name, const std::string& nameClose);
  std::map<std::string, std::shared_ptr<AbstractCNNFinder> > createCNNMap();


private: // for debugging
  Stopwatch stopwatch;
  std::vector<double> stopwatch_values;
  RingBufferWithSum<double, 100> mean_of_means;

private:
  CameraInfo::CameraID cameraID;

  DOUBLE_CAM_PROVIDE(MultiPassBallDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(MultiPassBallDetector, Image);
  DOUBLE_CAM_REQUIRE(MultiPassBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(MultiPassBallDetector, FieldPercept);
  //DOUBLE_CAM_REQUIRE(MultiPassBallDetector, BodyContour);
  DOUBLE_CAM_REQUIRE(MultiPassBallDetector, BallDetectorIntegralImage);
  DOUBLE_CAM_REQUIRE(MultiPassBallDetector, FieldColorPercept);

  DOUBLE_CAM_PROVIDE(MultiPassBallDetector, BallCandidates);
};//end class MultiPassBallDetector

#endif // _MultiPassBallDetector_H_
