/**
* @file IntegralImageProvider.h
*
* Definition of class IntegralImageProvider
*/

#ifndef _IntegralImageProvider_H_
#define _IntegralImageProvider_H_

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


BEGIN_DECLARE_MODULE(IntegralImageProvider)
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

  void integralBild();
  void integralBildBottom();

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

    if(getFieldColorPercept().greenHSISeparator.isColor(pixel)) {
      ++akk[1];
    } else {
      akk[0] += pixel.y;
    }
  }

private:     
  
  DOUBLE_CAM_PROVIDE(IntegralImageProvider, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(IntegralImageProvider, Image);
  DOUBLE_CAM_REQUIRE(IntegralImageProvider, FieldColorPercept);

};//end class IntegralImageProvider


template<class ImageType>
void IntegralImageProvider::makeIntegralBild(ImageType& integralImage) const
{
  const int32_t FACTOR = integralImage.FACTOR;
  const uint32_t MAX_COLOR = ImageType::MAX_COLOR;

  const uint32_t imgWidth = getImage().width()/FACTOR;
	const uint32_t imgHeight = getImage().height()/FACTOR;
	integralImage.setDimension(imgWidth, imgHeight);

  uint32_t* dataPtr = integralImage.getDataPointer();

  uint32_t* prevRowPtr = dataPtr;
	uint32_t* curRowPtr  = dataPtr + imgWidth*MAX_COLOR;

  // NOTE: we use the byte-wise access to the image, so we skipp every 2nd pixel
  const Pixel* imgPtr = reinterpret_cast<Pixel*>(getImage().data());
  
  const int32_t FACTOR_HALF = integralImage.FACTOR/2;
  // initial offset in the y direction
  const uint32_t yImageOffsetInitial = getImage().width() * FACTOR_HALF;
  // In each line we iterate over imgWidth-1 pixel. Because the pixel pointer is accumulated, 
  // we have to subtract it from the subsequent offset in the y-axis
  const uint32_t yImageOffsetIteration = yImageOffsetInitial - (imgWidth-1) * FACTOR_HALF;

  // initial y-offset
  imgPtr += yImageOffsetInitial;

  for(uint16_t y = 1; y < imgHeight; ++y) 
  {
    uint32_t akk[MAX_COLOR] = { 0 };

    prevRowPtr += MAX_COLOR;
    curRowPtr  += MAX_COLOR;

    for(uint16_t x = 1; x < imgWidth; ++x) 
    {
      imgPtr += FACTOR_HALF;

      if(getFieldColorPercept().greenHSISeparator.isColor(*imgPtr)) {
        ++akk[1];
      } else {
        akk[0] += (imgPtr->y0);
      }

      for(uint32_t i = 0; i < MAX_COLOR; ++i) {
        curRowPtr[i] = akk[i] + prevRowPtr[i];
      }

      curRowPtr  += MAX_COLOR;
      prevRowPtr += MAX_COLOR;
    }

    // iteration y-offset
    imgPtr += yImageOffsetIteration;
  }
}

#endif // _IntegralImageProvider_H_
