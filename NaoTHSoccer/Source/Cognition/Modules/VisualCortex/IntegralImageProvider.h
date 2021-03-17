/**
* @file IntegralImageProvider.h
*
* Definition of class IntegralImageProvider
*/

#ifndef _IntegralImageProvider_H_
#define _IntegralImageProvider_H_

#include <ModuleFramework/Module.h>

#include <Tools/Math/Vector2.h>

// representations
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"

//#include "Representations/Perception/BodyContour.h"
//#include "Representations/Perception/ColorTable64.h"
//#include "Representations/Perception/FieldPercept.h"

// tools
#include "Tools/DoubleCamHelpers.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
//#include "Tools/Debug/DebugParameterList.h"


BEGIN_DECLARE_MODULE(IntegralImageProvider)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  //PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)

  //REQUIRE(ColorTable64)

  // HACK
  //REQUIRE(BodyContour)

  //PROVIDE(GameColorIntegralImage)
  //PROVIDE(GameColorIntegralImageTop)

  PROVIDE(BallDetectorIntegralImage)
  PROVIDE(BallDetectorIntegralImageTop)
END_DECLARE_MODULE(IntegralImageProvider)


class IntegralImageProvider: private IntegralImageProviderBase
{
public:
  IntegralImageProvider();
  ~IntegralImageProvider();

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }
 
private:
  CameraInfo::CameraID cameraID;

  void makeIntegralBild(BallDetectorIntegralImage& integralImage) const;
  void makeIntegralBildNew(BallDetectorIntegralImage& integralImage) const;

  /*
  void makeChannels(const GameColorIntegralImage& integralImage, const Vector2i& point, uint32_t (&akk) [GameColorIntegralImage::MAX_COLOR]) const {
    Pixel pixel;
    getImage().get_direct(point.x, point.y, pixel);
      
    ColorClasses::Color c = getColorTable64().getColorClass(pixel.y,pixel.u,pixel.v);
    if(c == ColorClasses::white) { 
      ++akk[0];
    } else if (c == ColorClasses::green || getBodyContour().isOccupied(point)){
      ++akk[1];
    } else if(c == ColorClasses::black) {
      ++akk[2];
    }
  }

  void makeChannels(const BallDetectorIntegralImage& integralImage, const Vector2i& point, uint32_t (&akk) [BallDetectorIntegralImage::MAX_COLOR]) const {
    Pixel pixel;
    getImage().get_direct(point.x, point.y, pixel);

    if(getFieldColorPercept().greenHSISeparator.isColor(pixel)) {
      ++akk[1];
    } else {
      akk[0] += pixel.y;
    }
  }
  */

private:     
  
  DOUBLE_CAM_PROVIDE(IntegralImageProvider, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(IntegralImageProvider, Image);
  DOUBLE_CAM_REQUIRE(IntegralImageProvider, FieldColorPercept);
  DOUBLE_CAM_PROVIDE(IntegralImageProvider, BallDetectorIntegralImage);
};//end class IntegralImageProvider


#endif // _IntegralImageProvider_H_
