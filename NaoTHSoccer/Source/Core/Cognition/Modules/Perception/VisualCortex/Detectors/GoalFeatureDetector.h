/**
* @file GoalFeatureDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class GoalFeatureDetector
*/

#ifndef _GoalFeatureDetector_H_
#define _GoalFeatureDetector_H_

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/FieldColorPercept.h"
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/GoalFeaturePercept.h"
#include "Representations/Perception/FieldPercept.h"

// tools
#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/Matrix_mxn.h"

#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/RingBuffer.h>
#include <Tools/DataStructures/RingBufferWithSum.h>

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"

#include <vector>

#include "Tools/naoth_opencv.h"

BEGIN_DECLARE_MODULE(GoalFeatureDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)

  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(FrameInfo)

  PROVIDE(GoalFeaturePercept)
  PROVIDE(GoalFeaturePerceptTop)
END_DECLARE_MODULE(GoalFeatureDetector)


class GoalFeatureDetector: private GoalFeatureDetectorBase
{
public:

  GoalFeatureDetector();
  virtual ~GoalFeatureDetector(){}

  // override the Module execute method
  virtual bool execute(CameraInfo::CameraID id, bool horizon = true);

  void execute()
  {
    bool topScanned = execute(CameraInfo::Top);

    if(!topScanned) {
      execute(CameraInfo::Top, false);
      if( !execute(CameraInfo::Bottom)) {
        execute(CameraInfo::Bottom, false);
      }
    }
  }

private:
  static const int imageBorderOffset = 5;
  CameraInfo::CameraID cameraID;

  RingBuffer<Vector2i, 5> pointBuffer;
  RingBufferWithSum<double, 5> valueBuffer;
  RingBufferWithSum<double, 5> valueBufferY;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("GoalFeatureParameters")
    {
      PARAMETER_REGISTER(numberOfScanlines) = 5;
      PARAMETER_REGISTER(scanlinesDistance) = 6;
      PARAMETER_REGISTER(thresholdUV) = 60;
      PARAMETER_REGISTER(thresholdY) = 140;

      //PARAMETER_REGISTER(maxFeatureDeviation) = 5;
      //PARAMETER_REGISTER(maxFootScanSquareError) = 4.0;
      //PARAMETER_REGISTER(minGoodPoints) = 3;
      //PARAMETER_REGISTER(footGreenScanSize) = 10;
      //PARAMETER_REGISTER(maxFeatureWidthError) = 0.2;
      //PARAMETER_REGISTER(enableFeatureWidthCheck) = false;
      //PARAMETER_REGISTER(enableGreenCheck) = false;

      PARAMETER_REGISTER(colorRegionDeviation) = 2;

      syncWithConfig();
      DebugParameterList::getInstance().add(this);
    }

    virtual ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    int numberOfScanlines;
    int scanlinesDistance;
    int thresholdUV;
    int thresholdY;

    //int maxFeatureDeviation;
    //double maxFootScanSquareError;
    //int minGoodPoints;

    //bool enableGreenCheck;
    //int footGreenScanSize; // number of pixels to scan for green below the footpoint
    //
    //double maxFeatureWidthError;
    //bool enableFeatureWidthCheck;

    double colorRegionDeviation;
  };

  Parameters params;

  void findFeatureCandidates(const Vector2d& scanDir, const Vector2d& p1, double threshold, double thresholdY);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, Image);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, ArtificialHorizon);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(GoalFeatureDetector, FieldPercept);

  DOUBLE_CAM_PROVIDE(GoalFeatureDetector, GoalFeaturePercept);

};//end class GoalFeatureDetector

#endif // _GoalFeatureDetector_H_
