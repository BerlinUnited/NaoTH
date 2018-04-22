/**
* @file BDRBallDetector.h
*
* Definition of class BDRBallDetector
*/

#ifndef _BDRBallDetector_H_
#define _BDRBallDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/ModuleManager.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/PointList.h>
#include <Tools/DataStructures/OccupancyGrid.h>
#include <Tools/DataStructures/Area.h>
#include <Tools/ImageProcessing/ColorModelConversions.h>

#include <Representations/Infrastructure/Image.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/MultiBallPercept.h"

#include "Representations/Perception/MultiChannelIntegralImage.h"

#include "Representations/Modeling/RobotPose.h"

// tools
#include "Tools/DoubleCamHelpers.h"
#include "Cognition/Modules/VisualCortex/BallDetector/Tools/BallKeyPointExtractor.h"


#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"


BEGIN_DECLARE_MODULE(BDRBallDetector)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  //REQUIRE(FieldPercept)
  //REQUIRE(FieldPerceptTop)
  //REQUIRE(FieldColorPercept)
  //REQUIRE(FieldColorPerceptTop)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)

  REQUIRE(BallDetectorIntegralImage)
  REQUIRE(BallDetectorIntegralImageTop)

  // needed to check if the ball is on the field
  REQUIRE(RobotPose)

  PROVIDE(MultiBallPercept)
END_DECLARE_MODULE(BDRBallDetector)


class BDRBallDetector: private BDRBallDetectorBase, private ModuleManager
{
public:
  BDRBallDetector();
  ~BDRBallDetector();

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    getMultiBallPercept().reset();
    
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }
 
private:
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("BDRBallDetector")
    {
      PARAMETER_REGISTER(keyDetector.borderRadiusFactorClose) = 0.5;
      PARAMETER_REGISTER(keyDetector.borderRadiusFactorFar) = 0.8;
      
      //PARAMETER_REGISTER(contrastMinimum) = 50;
      PARAMETER_REGISTER(minRedInsideRatio) = 0.5;
      PARAMETER_REGISTER(verifyByGlobalPosition) = true;

      syncWithConfig();
    }

    ~Parameters(){}
    
    BallKeyPointExtractor::Parameter keyDetector;

    //double contrastMinimum;
    double minRedInsideRatio;
    bool verifyByGlobalPosition;

  } params;


private:
  double calculateRedContrastIterative(int x0, int y0, int x1, int y1, int size);
  void addBallPercept(const Vector2i& center, double radius);

private: //data members
  ModuleCreator<BallKeyPointExtractor>* theBallKeyPointExtractor;
  BestPatchList best;

private:     
  
  DOUBLE_CAM_PROVIDE(BDRBallDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BDRBallDetector, Image);
  DOUBLE_CAM_REQUIRE(BDRBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(BDRBallDetector, BallDetectorIntegralImage);
  //DOUBLE_CAM_REQUIRE(BDRBallDetector, FieldColorPercept);
  //DOUBLE_CAM_REQUIRE(BDRBallDetector, FieldPercept);
          
};//end class BDRBallDetector

#endif // _BDRBallDetector_H_
