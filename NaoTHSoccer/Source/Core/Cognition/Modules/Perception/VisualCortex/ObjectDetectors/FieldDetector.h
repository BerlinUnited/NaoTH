/* 
 * File:   FieldDetector.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef __FieldDetector_H_
#define __FieldDetector_H_

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ArtificialHorizon.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"

BEGIN_DECLARE_MODULE(FieldDetector)
  REQUIRE(Image)
  REQUIRE(ImageTop)
  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(ArtificialHorizon)
  REQUIRE(ArtificialHorizonTop)

  PROVIDE(FieldPercept)
  PROVIDE(FieldPerceptTop)
END_DECLARE_MODULE(FieldDetector)


class FieldDetector : private FieldDetectorBase
{
public:
  FieldDetector();
  virtual ~FieldDetector();

  void execute()
  {
    execute(CameraInfo::Bottom);
  };

  void execute(CameraInfo::CameraID id);

private:
  CameraInfo::CameraID cameraID;

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

  const ScanLineEdgelPercept& getScanLineEdgelPercept_() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return getScanLineEdgelPerceptTop();
    }
    else
    {
      return getScanLineEdgelPercept();
    }
  };

  FieldPercept& getFieldPercept_() 
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
};

#endif  /* __FieldDetector_H_ */

