/**
* @file BDRIntegralImageProvider.cpp
*
* Implementation of class BDRIntegralImageProvider
*
*/

#include "BDRIntegralImageProvider.h"

BDRIntegralImageProvider::BDRIntegralImageProvider()
{
}

BDRIntegralImageProvider::~BDRIntegralImageProvider()
{
}

void BDRIntegralImageProvider::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  makeIntegralBild(getBallDetectorIntegralImage());
}

void BDRIntegralImageProvider::makeIntegralBild(BallDetectorIntegralImage& integralImage) const
{
  const int32_t FACTOR = integralImage.FACTOR;
  const uint32_t MAX_COLOR = integralImage.MAX_COLOR;

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
      } else if(getFieldColorPercept().redHSISeparator.isColor(*imgPtr)) {
        ++akk[2];
        akk[0] += imgPtr->v;
      } else {
        akk[0] += imgPtr->v;
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