/**
* @file BDRIntegralImageProvider.h
*
* Definition of class BDRIntegralImageProvider
*/

#ifndef _BDRIntegralImageProvider_H_
#define _BDRIntegralImageProvider_H_

#include <ModuleFramework/Module.h>
#include <ModuleFramework/Representation.h>

// common tools
//#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>

#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Perception/FieldColorPercept.h"
#include "Representations/Perception/MultiChannelIntegralImage.h"
#include "Representations/Perception/BodyContour.h"

// needed?
//#include "Representations/Perception/FieldPercept.h"
//#include "Representations/Perception/BodyContour.h"

// tools
#include "Tools/DoubleCamHelpers.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Representations/Perception/ColorTable64.h"


BEGIN_DECLARE_MODULE(BDRIntegralImageProvider)
  PROVIDE(DebugRequest)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(Image)
  REQUIRE(ImageTop)

  REQUIRE(FieldColorPercept)
  REQUIRE(FieldColorPerceptTop)

  REQUIRE(ColorTable64)

  // HACK
  REQUIRE(BodyContour)

  //PROVIDE(GameColorIntegralImage)
  //PROVIDE(GameColorIntegralImageTop)

  PROVIDE(BallDetectorIntegralImage)
  PROVIDE(BallDetectorIntegralImageTop)
END_DECLARE_MODULE(BDRIntegralImageProvider)


class BDRIntegralImageProvider: private BDRIntegralImageProviderBase
{
public:
  BDRIntegralImageProvider(){};
  ~BDRIntegralImageProvider(){};

  virtual void execute()
  {
    execute(CameraInfo::Top);
    execute(CameraInfo::Bottom);
  }
 
private:
  CameraInfo::CameraID cameraID;

  void execute(CameraInfo::CameraID id)
  {
    cameraID = id;
    if(cameraID == CameraInfo::Bottom) {
      makeIntegralBild(getBallDetectorIntegralImage());
    } else {
      makeIntegralBild(getBallDetectorIntegralImageTop());
    }
  }

private:

  template<class ImageType>
  void makeIntegralBild(ImageType& integralImage) const;

  void makeChannels(const GameColorIntegralImage& /*integralImage*/, const Vector2i& point, uint32_t (&akk) [GameColorIntegralImage::MAX_COLOR]) const {
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

  void makeChannels(const BallDetectorIntegralImage& /*integralImage*/, const Vector2i& point, uint32_t (&akk) [BallDetectorIntegralImage::MAX_COLOR]) const {
    Pixel pixel;
    getImage().get_direct(point.x, point.y, pixel);

    if(!getFieldColorPercept().greenHSISeparator.noColor(pixel)) {
      if(getFieldColorPercept().greenHSISeparator.isChroma(pixel)) {
        ++akk[1];
      } else if(getFieldColorPercept().redHSISeparator.isChroma(pixel)) {
        ++akk[2];
        akk[0] += pixel.v;
      }
    } else {
      akk[0] += pixel.v;
    }
  }

private:     
  
  DOUBLE_CAM_PROVIDE(BDRIntegralImageProvider, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(BDRIntegralImageProvider, Image);
  DOUBLE_CAM_REQUIRE(BDRIntegralImageProvider, FieldColorPercept);

};//end class BDRIntegralImageProvider


template<class ImageType>
void BDRIntegralImageProvider::makeIntegralBild(ImageType& integralImage) const
{
  const int32_t FACTOR = integralImage.FACTOR;
  const uint32_t MAX_COLOR = ImageType::MAX_COLOR;

  const uint32_t imgWidth = getImage().width()/FACTOR;
	const uint32_t imgHeight = getImage().height()/FACTOR;
	integralImage.setDimension(imgWidth, imgHeight);

  uint32_t* dataPtr = integralImage.getDataPointer();

  uint32_t* prevRowPtr = dataPtr;
	uint32_t* curRowPtr  = dataPtr + imgWidth*MAX_COLOR;

  Vector2i p;
  
  for(uint16_t y = 1; y < imgHeight; ++y) 
  {
    uint32_t akk[MAX_COLOR] = { 0 };

    prevRowPtr += MAX_COLOR;
    curRowPtr  += MAX_COLOR;

    for(uint16_t x = 1; x < imgWidth; ++x) 
    {
#ifndef WIN32
      // preload the cache, so the next row(s) are loaded while we are busy
			// still working with them (https://gcc.gnu.org/onlinedocs/gcc-4.8.2/gcc/Other-Builtins.html)
			__builtin_prefetch(&prevRowPtr[64], 0);
#endif
      p.x = x*FACTOR;
      p.y = y*FACTOR;

      makeChannels(integralImage, p, akk);

      for(uint32_t i = 0; i < MAX_COLOR; ++i) {
        curRowPtr[i] = akk[i] + prevRowPtr[i];
      }

      curRowPtr  += MAX_COLOR;
      prevRowPtr += MAX_COLOR;
    }
  }
}

#endif // _BDRIntegralImageProvider_H_
