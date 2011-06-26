/**
* @file ImageProcessor.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class ImageProcessor
*/

#ifndef __ImageProcessor_H_
#define __ImageProcessor_H_

// infrastructure
#include <ModuleFramework/Representation.h>
#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

//local Tools
#include "Tools/ImageProcessing/BlobFinder.h"
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histogram.h"

// submodules
#include "ObjectDetectors/ScanLineEdgelDetector.h"
#include "ObjectDetectors/FieldDetector.h"
#include "ObjectDetectors/BallDetector.h"
#include "ObjectDetectors/LineDetector.h"
#include "ObjectDetectors/RobotDetector.h"
#include "ObjectDetectors/GoalDetector.h"
#include "PerceptionsVisualization.h"


// Representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Infrastructure/ColorTable64.h"
//#include "Representations/Perception/BlobPercept.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/PlayersPercept.h"
#include "Representations/Perception/CameraMatrix.h"


#include <sstream>

BEGIN_DECLARE_MODULE(ImageProcessor)
  REQUIRE(ColorTable64)
  REQUIRE(CameraMatrix)
  REQUIRE(Image)
  REQUIRE(FrameInfo)
  REQUIRE(FieldInfo)

  PROVIDE(BallPercept)
  PROVIDE(PlayersPercept)
  PROVIDE(LinePercept)
  PROVIDE(GoalPercept)
  PROVIDE(ScanLineEdgelPercept)
END_DECLARE_MODULE(ImageProcessor)

class ImageProcessor: private ImageProcessorBase, private ModuleManager
{
public:
  ImageProcessor();
  ~ImageProcessor(){};

  virtual void execute();

private:
  ModuleCreator<ScanLineEdgelDetector>* theScanLineEdgelDetector;
  ModuleCreator<FieldDetector>* theFieldDetector;

  ModuleCreator<BallDetector>* theBallDetector;
  ModuleCreator<RobotDetector>* theRobotDetector;
  
  ModuleCreator<LineDetector>* theLineDetector;
  ModuleCreator<GoalDetector>* theGoalDetector;
  ModuleCreator<PerceptionsVisualization>* thePerceprionsVusalization;
};//end class ImageProcessor

#endif // __ImageProcessor_H_
