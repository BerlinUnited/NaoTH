/**
* @file BDRBallDetector.h
*
* Definition of class BDRBallDetector
*/

#ifndef _BDRBallDetector_H_
#define _BDRBallDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

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
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/Histograms.h"
#include "Representations/Perception/MultiBallPercept.h"

// tools
#include "Tools/DoubleCamHelpers.h"

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
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)

  PROVIDE(MultiBallPercept)
END_DECLARE_MODULE(BDRBallDetector)


class BDRBallDetector: private BDRBallDetectorBase
{
public:
  BDRBallDetector();
  ~BDRBallDetector();

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    getMultiBallPercept().reset();
    
    cameraID = CameraInfo::Top;
    execute(cameraID);

    cameraID = CameraInfo::Bottom;
    execute(cameraID);
  }
 
private:
  CameraInfo::CameraID cameraID;

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("BDRBallDetector")
    {
      PARAMETER_REGISTER(stepSize) = 2;
      
      syncWithConfig();
    }

    ~Parameters(){}

	  int stepSize;

  } params;


private:

private: //data members

private:     
  
  DOUBLE_CAM_PROVIDE(BDRBallDetector, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BDRBallDetector, Image);
  DOUBLE_CAM_REQUIRE(BDRBallDetector, CameraMatrix);
  DOUBLE_CAM_REQUIRE(BDRBallDetector, FieldColorPercept);
  DOUBLE_CAM_REQUIRE(BDRBallDetector, FieldPercept);
          
};//end class BDRBallDetector

#endif // _BDRBallDetector_H_
