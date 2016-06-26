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

  PROVIDE(GameColorIntegralImage)
  PROVIDE(GameColorIntegralImageTop)
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
  void makeGameColorIntegralBild(ImageType& integralImage) const;

private:     
  
  DOUBLE_CAM_PROVIDE(IntegralImageProvider, DebugImageDrawings);

  // double cam stuff
  DOUBLE_CAM_REQUIRE(IntegralImageProvider, Image);
  DOUBLE_CAM_REQUIRE(IntegralImageProvider, FieldColorPercept);

};//end class IntegralImageProvider


template<class ImageType>
void IntegralImageProvider::makeGameColorIntegralBild(ImageType& integralImage) const
{
  ASSERT(ImageType::MAX_COLOR >= 3);
  const int32_t FACTOR = integralImage.FACTOR;
  const uint32_t MAX_COLOR = ImageType::MAX_COLOR;

  const uint imgWidth = getImage().width()/FACTOR;
	const uint imgHeight = getImage().height()/FACTOR;
	integralImage.setDimension(imgWidth, imgHeight);

  uint32_t* dataPtr = integralImage.getDataPointer();

  uint32_t* prevRowPtr = dataPtr;
	uint32_t* curRowPtr  = dataPtr + imgWidth*MAX_COLOR;

  Pixel pixel;
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
      getImage().get_direct(p.x, p.y, pixel);
      
      ColorClasses::Color c = getColorTable64().getColorClass(pixel.y,pixel.u,pixel.v);
      if(c == ColorClasses::white) { 
        ++akk[0];
      } else if (c == ColorClasses::green || getBodyContour().isOccupied(p)){
        ++akk[1];
      } else if(c == ColorClasses::black) {
        ++akk[2];
      }

      for(uint32_t i = 0; i < MAX_COLOR; ++i) {
        curRowPtr[i] = akk[i] + prevRowPtr[i];
      }

      curRowPtr  += MAX_COLOR;
      prevRowPtr += MAX_COLOR;
    }
  }
}

#endif // _IntegralImageProvider_H_
