/**
* @file BDRIntegralImageProvider.h
*
* Definition of class BDRIntegralImageProvider
*/

#ifndef _BDRIntegralImageProvider_H_
#define _BDRIntegralImageProvider_H_

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


BEGIN_DECLARE_MODULE(BDRIntegralImageProvider)
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
END_DECLARE_MODULE(BDRIntegralImageProvider)


class BDRIntegralImageProvider: private BDRIntegralImageProviderBase
{
public:
  BDRIntegralImageProvider();
  ~BDRIntegralImageProvider();

  void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }
 
private:
  CameraInfo::CameraID cameraID;

  void makeIntegralBild(BallDetectorIntegralImage& integralImage) const;

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
  
  DOUBLE_CAM_PROVIDE(BDRIntegralImageProvider, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BDRIntegralImageProvider, Image);
  DOUBLE_CAM_REQUIRE(BDRIntegralImageProvider, FieldColorPercept);
  DOUBLE_CAM_PROVIDE(BDRIntegralImageProvider, BallDetectorIntegralImage);
};//end class BDRIntegralImageProvider


#endif // _BDRIntegralImageProvider_H_
