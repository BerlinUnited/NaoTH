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

  const CameraMatrix& getCameraMatrix() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return FieldDetectorBase::getCameraMatrixTop();
    }
    else
    {
      return FieldDetectorBase::getCameraMatrix();
    }
  };

  const Image& getImage() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return FieldDetectorBase::getImageTop();
    }
    else
    {
      return FieldDetectorBase::getImage();
    }
  };

  const ArtificialHorizon& getArtificialHorizon() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return FieldDetectorBase::getArtificialHorizonTop();
    }
    else
    {
      return FieldDetectorBase::getArtificialHorizon();
    }
  };

  const ScanLineEdgelPercept& getScanLineEdgelPercept() const
  {
    if(cameraID == CameraInfo::Top)
    {
      return FieldDetectorBase::getScanLineEdgelPerceptTop();
    }
    else
    {
      return FieldDetectorBase::getScanLineEdgelPercept();
    }
  };

  FieldPercept& getFieldPercept() 
  {
    if(cameraID == CameraInfo::Top)
    {
      return FieldDetectorBase::getFieldPerceptTop();
    }
    else
    {
      return FieldDetectorBase::getFieldPercept();
    }
  };
};

#endif  /* __FieldDetector_H_ */

