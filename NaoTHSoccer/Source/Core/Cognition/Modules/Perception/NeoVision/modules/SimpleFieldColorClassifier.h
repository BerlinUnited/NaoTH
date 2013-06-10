/* 
 * File:   SimpleFieldColorClassifier.h
 * Author: claas
 * Author: Heinrich Mellmann
 *
 * Created on 15. März 2011, 15:56
 */

#ifndef _SimpleFieldColorClassifier_H_
#define _SimpleFieldColorClassifier_H_

#include <ModuleFramework/Module.h>

// Representations
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Tools/ImageProcessing/Histograms.h"
//#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Representations/Infrastructure/Image.h" // just for debug


// Tools
#include "Tools/Math/Vector2.h"
#include "Tools/Math/Vector3.h"
#include <Tools/DataStructures/ParameterList.h>

#include "Tools/Debug/DebugImageDrawings.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(SimpleFieldColorClassifier)
  //REQUIRE(ColoredGrid)
  REQUIRE(FrameInfo)
  REQUIRE(Histograms)
  REQUIRE(HistogramsTop)
  REQUIRE(Image)
  REQUIRE(ImageTop)

  PROVIDE(FieldColorPercept)
  PROVIDE(FieldColorPerceptTop)
END_DECLARE_MODULE(SimpleFieldColorClassifier)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////


class SimpleFieldColorClassifier : public  SimpleFieldColorClassifierBase
{
public:
  SimpleFieldColorClassifier();
  virtual ~SimpleFieldColorClassifier(){}

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);

    // reset the debug drawing canvas to bottom
    CANVAS_PX_BOTTOM;
  }

  void execute(const CameraInfo::CameraID id);

private:

  class Parameters: public ParameterList
  {
  public:

    Parameters() : ParameterList("SimpleFieldColorClassifierParameters")
    {
      PARAMETER_REGISTER(fieldColorMax.y) = 64;
      PARAMETER_REGISTER(fieldColorMin.y) = 64;
      PARAMETER_REGISTER(fieldColorMax.u) = 8;
      PARAMETER_REGISTER(fieldColorMax.v) = 10;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters()
    {
      DebugParameterList::getInstance().remove(this);
    }

    DoublePixel fieldColorMax;
    DoublePixel fieldColorMin;
  };

  Parameters fieldParams;

  // id of the camera the module is curently running on
  CameraInfo::CameraID cameraID;

  const Image& getImage() const
  {
    if(cameraID == CameraInfo::Top) {
      return SimpleFieldColorClassifierBase::getImageTop();
    } else {
      return SimpleFieldColorClassifierBase::getImage();
    }
  }

  const Histograms& getHistograms() const
  {
    if(cameraID == CameraInfo::Top) {
      return SimpleFieldColorClassifierBase::getHistogramsTop();
    } else {
      return SimpleFieldColorClassifierBase::getHistograms();
    }
  }

  FieldColorPercept& getFieldColorPercept() const
  {
    if(cameraID == CameraInfo::Top) {
      return SimpleFieldColorClassifierBase::getFieldColorPerceptTop();
    } else {
      return SimpleFieldColorClassifierBase::getFieldColorPercept();
    }
  }
};

#endif  /* _SimpleFieldColorClassifier_H_ */