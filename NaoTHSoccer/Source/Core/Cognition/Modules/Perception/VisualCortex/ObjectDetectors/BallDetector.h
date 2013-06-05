/**
* @file BallDetector.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class BallDetector
*/

#ifndef __BallDetector_H_
#define __BallDetector_H_

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

#include "Representations/Perception/BodyContour.h"
#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Infrastructure/FieldInfo.h"
//#include "Representations/Perception/BlobPercept.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Modeling/ColorClassificationModel.h"

#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/BallPercept.h"

// tools
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/BlobFinder.h"
#include "Tools/ImageProcessing/SpiderScan.h"



BEGIN_DECLARE_MODULE(BallDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(ColoredGrid)
  REQUIRE(ColoredGridTop)
//  REQUIRE(ColorTable64)
  REQUIRE(ColorClassificationModel)
  REQUIRE(ColorClassificationModelTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)
  REQUIRE(KinematicChain)
  REQUIRE(FieldPercept)
  REQUIRE(FieldPerceptTop)
  REQUIRE(BodyContour)
  REQUIRE(BodyContourTop)
//  REQUIRE(BlobPercept)
  REQUIRE(FieldInfo)

  PROVIDE(BallPercept)
  PROVIDE(BallPerceptTop)
END_DECLARE_MODULE(BallDetector)


class BallDetector: private BallDetectorBase
{
public:
  BallDetector();
  ~BallDetector(){};

  void execute(const Vector2<int>& start);

  // override the Module execute method
  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
     execute(CameraInfo::Bottom);
  };
 
private:
  CameraInfo::CameraID cameraID;
  typedef PointList<20> BallPointList;

  bool connectedColors[ColorClasses::numOfColors];
  BlobFinder theBlobFinder;
  BlobFinder theBlobFinderTop;

  bool calculateCircle( const BallPointList& ballPoints, Vector2<double>& center, double& radius );
  bool randomScan(ColorClasses::Color color, Vector2<int>& result, const Vector2<int>& orgMin, const Vector2<int>& orgMax) const;

  void regionGrowExpandArea( const Vector2<int>& startingPoint, Vector2<double>& result, double& radius);

  double calculateBase(Vector2<int>& x, Vector2<int>& y, Vector2<int>& z);

  BlobFinder& getBlobFinder()
  {
    if(cameraID == CameraInfo::Top)
    {
      return theBlobFinderTop;
    }
    else
    {
      return theBlobFinder;
    }
  }

  const ColorClassificationModel& getColorTable64() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getColorClassificationModelTop();
    }
    else
    {
      return getColorClassificationModel();
    }
  };

  const Image& getImage_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getImageTop();
    }
    else
    {
      return getImage();
    }
  };
  
  const ColoredGrid& getColoredGrid_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getColoredGridTop();
    }
    else
    {
      return getColoredGrid();
    }
  };

  const CameraMatrix& getCameraMatrix_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getCameraMatrixTop();
    }
    else
    {
      return getCameraMatrix();
    }
  };

  const CameraInfo& getCameraInfo_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getCameraInfoTop();
    }
    else
    {
      return getCameraInfo();
    }
  };

  const ArtificialHorizon& getArtificialHorizon_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getArtificialHorizonTop();
    }
    else
    {
      return getArtificialHorizon();
    }
  };

  const FieldPercept& getFieldPercept_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getFieldPerceptTop();
    }
    else
    {
      return getFieldPercept();
    }
  };

  const BodyContour& getBodyContour_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getBodyContourTop();
    }
    else
    {
      return getBodyContour();
    }
  };
  
  BallPercept& getBallPercept_()
  {
    if(cameraID == CameraInfo::Top)
    {
      return getBallPerceptTop();
    }
    else
    {
      return getBallPercept();
    }
  };
          
};//end class BallDetector

#endif // __BallDetector_H_
