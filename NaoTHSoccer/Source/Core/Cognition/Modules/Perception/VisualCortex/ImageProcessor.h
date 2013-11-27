/**
* @file ImageProcessor.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class ImageProcessor
*/

#ifndef _ImageProcessor_H_
#define _ImageProcessor_H_

// infrastructure
#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

//local Tools
#include "Tools/ImageProcessing/BlobFinder.h"
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histograms.h"

// submodules
#include "ObjectDetectors/HistogramFieldDetector.h"
#include "ObjectDetectors/ScanLineEdgelDetector.h"
#include "ObjectDetectors/FieldDetector.h"
#include "ObjectDetectors/BallDetector.h"
#include "ObjectDetectors/LineDetector.h"
#include "ObjectDetectors/LineClusterProvider.h"
#include "ObjectDetectors/RobotDetector.h"
#include "ObjectDetectors/GoalDetector.h"

#include "Cognition/Modules/Perception/BodyContourProvider/BodyContourProvider.h"


// TODO: this is from NeoVision
#include "Cognition/Modules/Perception/NeoVision/modules/SimpleFieldColorClassifier.h"
#include "Cognition/Modules/Perception/NeoVision/modules/ScanLineEdgelDetectorDifferential.h"
#include "Cognition/Modules/Perception/NeoVision/modules/MaximumRedBallDetector.h"
#include "Cognition/Modules/Perception/NeoVision/modules/GradientGoalDetector.h"


// Representations
#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/ArtificialHorizon.h"

#include <sstream>

BEGIN_DECLARE_MODULE(ImageProcessor)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
END_DECLARE_MODULE(ImageProcessor)

class ImageProcessor: private ImageProcessorBase, private ModuleManager
{
public:
  ImageProcessor();
  ~ImageProcessor(){}

  virtual void execute();

private:
  ModuleCreator<HistogramFieldDetector>* theHistogramFieldDetector;
  ModuleCreator<BodyContourProvider>* theBodyContourProvider;
  ModuleCreator<ScanLineEdgelDetector>* theScanLineEdgelDetector;
  ModuleCreator<FieldDetector>* theFieldDetector;

  ModuleCreator<BallDetector>* theBallDetector;
  ModuleCreator<RobotDetector>* theRobotDetector;
  ModuleCreator<LineDetector>* theLineDetector;
  ModuleCreator<LineClusterProvider>* theLineClusterProvider;
  ModuleCreator<GoalDetector>* theGoalDetector;


  // TODO: NeoVision stuff
  ModuleCreator<SimpleFieldColorClassifier>* theSimpleFieldColorClassifier;
  ModuleCreator<ScanLineEdgelDetectorDifferential>* theScanLineEdgelDetectorDifferential;
  ModuleCreator<MaximumRedBallDetector>* theMaximumRedBallDetector;
  ModuleCreator<GradientGoalDetector>* theGradientGoalDetector;

};//end class ImageProcessor

#endif // _ImageProcessor_H_
