/**
* @file IntegralImageProvider.cpp
*
* Implementation of class IntegralImageProvider
*
*/

#include "IntegralImageProvider.h"

IntegralImageProvider::IntegralImageProvider()
{
}

IntegralImageProvider::~IntegralImageProvider()
{
}

void IntegralImageProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  if(cameraID == CameraInfo::Bottom) {
    integralBildBottom();
  } else {
    integralBild();
  }
}


void IntegralImageProvider::integralBild()
{
  const int32_t FACTOR = getMultiChannelIntegralImage().FACTOR;
  const uint32_t MAX_COLOR = MultiChannelIntegralImage::MAX_COLOR;
  const uint imgWidth = getImage().width()/FACTOR;
	const uint imgHeight = getImage().height()/FACTOR;
	getMultiChannelIntegralImage().setDimension(imgWidth, imgHeight);

  uint32_t* dataPtr = getMultiChannelIntegralImage().getDataPointer();

  uint32_t* prevRowPtr = dataPtr;
	uint32_t* curRowPtr  = dataPtr + imgWidth*MAX_COLOR;

  Pixel pixel;
  
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
      getImage().get_direct(x*FACTOR, y*FACTOR, pixel);
      
      ColorClasses::Color c = getColorTable64().getColorClass(pixel.y,pixel.u,pixel.v);
      if(c == ColorClasses::white) { 
        ++akk[0];
      } else if (c == ColorClasses::green){
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

void IntegralImageProvider::integralBildBottom()
{
  const int32_t FACTOR = getMultiChannelIntegralImage().FACTOR;
  const uint32_t MAX_COLOR = MultiChannelIntegralImage::MAX_COLOR;
  const uint imgWidth = getImage().width()/FACTOR;
	const uint imgHeight = getImage().height()/FACTOR;
	getMultiChannelIntegralImage().setDimension(imgWidth, imgHeight);

  uint32_t* dataPtr = getMultiChannelIntegralImage().getDataPointer();

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
