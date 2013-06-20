/**
* @file MaximumRedBallDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:critter@informatik.hu-berlin.de">CNR</a>
* Definition of class MaximumRedBallDetector
*/

#ifndef __MaximumRedBallDetector_H_
#define __MaximumRedBallDetector_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

// common tools
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Matrix_nxn.h>
#include <Tools/Math/PointList.h>
#include <Tools/DataStructures/OccupancyGrid.h>
#include <Tools/DataStructures/Area.h>
#include <Tools/DataStructures/ParameterList.h>
#include <Tools/ImageProcessing/ColorModelConversions.h>

#include <Representations/Infrastructure/Image.h>

#include "Representations/Perception/BodyContour.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/FieldPercept.h"
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BallPercept.h"
#include "Representations/Perception/FieldColorPercept.h"

// tools
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/GradientSpiderScan.h"

BEGIN_DECLARE_MODULE(MaximumRedBallDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(KinematicChain)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
  REQUIRE(FieldInfo)
  REQUIRE(FrameInfo)

  PROVIDE(BallPercept)
  PROVIDE(BallPerceptTop)
END_DECLARE_MODULE(MaximumRedBallDetector)


class MaximumRedBallDetector: private MaximumRedBallDetectorBase
{
public:
  MaximumRedBallDetector();
  ~MaximumRedBallDetector(){};

  // override the Module execute method
  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
     execute(CameraInfo::Top);
     execute(CameraInfo::Bottom);
  };
 
private:
  CameraInfo::CameraID cameraID;
  typedef PointList<20> BallPointList;
  BallPointList goodPoints;
  BallPointList badPoints;

  BallPointList bestPoints;

  BallPointList possibleModells[10];

  void findMaximumRedPoint(Vector2<int>& peakPos);
  Vector2d estimatePositionBySize();
  bool calculateCircle( const BallPointList& ballPoints, Vector2<double>& center, double& radius );
  
  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("MaximumRedBallDetectorParameters")
    {
      PARAMETER_REGISTER(gradientThreshold) = 30;
      PARAMETER_REGISTER(meanThreshold) = 30;
      PARAMETER_REGISTER(stepSize) = 4;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    int meanThreshold;
    int gradientThreshold;
    int stepSize;
  };

  Parameters params;

  const Image& getImage() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return MaximumRedBallDetectorBase::getImageTop();
    }
    else
    {
      return MaximumRedBallDetectorBase::getImage();
    }
  };
  
  const CameraMatrix& getCameraMatrix() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return MaximumRedBallDetectorBase::getCameraMatrixTop();
    }
    else
    {
      return MaximumRedBallDetectorBase::getCameraMatrix();
    }
  };

  const CameraInfo& getCameraInfo() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return MaximumRedBallDetectorBase::getCameraInfoTop();
    }
    else
    {
      return MaximumRedBallDetectorBase::getCameraInfo();
    }
  };

  const ArtificialHorizon& getArtificialHorizon() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return MaximumRedBallDetectorBase::getArtificialHorizonTop();
    }
    else
    {
      return MaximumRedBallDetectorBase::getArtificialHorizon();
    }
  };

  const FieldColorPercept& getFieldColorPercept() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return MaximumRedBallDetectorBase::getFieldColorPerceptTop();
    }
    else
    {
      return MaximumRedBallDetectorBase::getFieldColorPercept();
    }
  };

  const FieldPercept& getFieldPercept() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return MaximumRedBallDetectorBase::getFieldPerceptTop();
    }
    else
    {
      return MaximumRedBallDetectorBase::getFieldPercept();
    }
  };

  const BodyContour& getBodyContour() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return MaximumRedBallDetectorBase::getBodyContourTop();
    }
    else
    {
      return MaximumRedBallDetectorBase::getBodyContour();
    }
  };
  
  BallPercept& getBallPercept()
  {
    if(cameraID == CameraInfo::Top)
    {
      return MaximumRedBallDetectorBase::getBallPerceptTop();
    }
    else
    {
      return MaximumRedBallDetectorBase::getBallPercept();
    }
  };
          
};//end class MaximumRedBallDetector

#endif // __MaximumRedBallDetector_H_
