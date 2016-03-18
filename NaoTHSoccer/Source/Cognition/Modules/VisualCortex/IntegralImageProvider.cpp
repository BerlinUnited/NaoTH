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
  integralBild();
}


void IntegralImageProvider::integralBild()
{
  const uint imgWidth = getImage().width()/FACTOR;
	const uint imgHeight = getImage().height()/FACTOR;
	getGameColorIntegralImage().setDimension(imgWidth, imgHeight);

  uint32_t* dataPtr = getGameColorIntegralImage().getDataPointer();

  uint32_t* prevRowPtr = dataPtr;
	uint32_t* curRowPtr  = dataPtr + imgWidth;

  Pixel pixel;
  
  for(uint16_t y = 1; y < imgHeight; ++y) 
  {
    uint16_t akk = 0;

    prevRowPtr += 1;
		curRowPtr  += 1;

    for(uint16_t x = 1; x < imgWidth; ++x) 
    {
      getImage().get(x*FACTOR, y*FACTOR, pixel);
      
      if(getFieldColorPercept().greenHSISeparator.noColor(pixel)) {
        ++akk;
      }

      curRowPtr[0] = akk + prevRowPtr[0];

      curRowPtr  += 1;
	    prevRowPtr += 1;
    }
  }
}
