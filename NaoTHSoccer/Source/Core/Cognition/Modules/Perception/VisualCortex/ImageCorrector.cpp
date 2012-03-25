/* 
 * File:   ImageCorrector.cpp
 * Author: claas
 * 
 * Created on 22. November 2010, 19:37
 */

#include "ImageCorrector.h"

ImageCorrector::ImageCorrector()
:
  correctionCycle(0),
  correctorIsRunning(false)
{
  DEBUG_REQUEST_REGISTER("Image:Corrector:correctBrighnessInImage", " ", false);
  DEBUG_REQUEST_REGISTER("Image:Corrector:simpleBrighnessCorrection", " ", false);
  DEBUG_REQUEST_REGISTER("Image:Corrector:GaussianBlur", " ", false);
  DEBUG_REQUEST_REGISTER("Image:Corrector:reset", " ", false);
  DEBUG_REQUEST_REGISTER("Image:Corrector:saveToFile", " ", false);
  DEBUG_REQUEST_REGISTER("Image:Corrector:loadFromFile", " ", false);
}

ImageCorrector::~ImageCorrector()
{
}

void ImageCorrector::execute()
{
  simpleBrighnessCorrectionRequestActive = false;
  DEBUG_REQUEST("Image:Corrector:simpleBrighnessCorrection",
    if(!correctorIsRunning)  
    {
      correctionCycle = 255;
    }
    simpleBrighnessCorrectionRequestActive = true;
  );

  if(correctionCycle > 0 && correctionCycle < 256)
  {
    correctorIsRunning = true;
    correctBrightnessSimple();
    correctionCycle--;
  }
  else if(!simpleBrighnessCorrectionRequestActive)
  {
    correctorIsRunning = false;
  }

  DEBUG_REQUEST("Image:Corrector:reset",
  getImage().shadingCorrection.reset();
  );

  DEBUG_REQUEST("Image:Corrector:correctBrighnessInImage",
    for(unsigned int i = 0; i < getImage().cameraInfo.size; i++)
    {
      unsigned int newValue = (getImage().shadingCorrection.getY(i) * getImage().yuv422[2 * i]) >> 10;
      newValue = Math::clamp<unsigned int>(newValue, 0, 255);
      getImage().yuv422[2 * i] = (unsigned char) newValue;
    }
  );

  DEBUG_REQUEST("Image:Corrector:saveToFile",
    getImage().shadingCorrection.saveCorrectionToFile
    (
      Platform::getInstance().theConfigDirectory,
      Platform::getInstance().theHeadHardwareIdentity
    );
  );

  DEBUG_REQUEST("Image:Corrector:loadFromFile",
    getImage().shadingCorrection.loadCorrectionFromFile
    (
      Platform::getInstance().theConfigDirectory,
      Platform::getInstance().theHeadHardwareIdentity
    );
  );
}

void ImageCorrector::correctBrightnessSimple()
{
  double mX = 0.5f * (double) getImage().width();
  double mY = 0.5f * (double) getImage().height();

  unsigned int x1 = (unsigned int) ceil(mX);
  unsigned int x2 = (unsigned int) floor(mX);
  unsigned int y1 = (unsigned int) ceil(mY);
  unsigned int y2 = (unsigned int) floor(mY);

  unsigned int meanMiddleBrighness = (getImage().getPlainY(x1,y1) + getImage().getPlainY(x1,y2) + getImage().getPlainY(x2,y1) + getImage().getPlainY(x2,y2));

  for(unsigned int y = 0; y < getImage().height(); y++)
  {
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      unsigned int pixelY = getImage().getPlainY(x, y);
      unsigned int corrY = getImage().shadingCorrection.getY(x, y);
      if(pixelY > 0)
      {
        if(correctionCycle != 255)
        {
          getImage().shadingCorrection.setY(x, y, ((255 *  meanMiddleBrighness / pixelY) + corrY) / 2 );
        }
        else
        {
          getImage().shadingCorrection.setY(x, y, 255 *  meanMiddleBrighness / pixelY);
        }
      }
      else
      {
        if(correctionCycle != 255)
        {
          getImage().shadingCorrection.setY(x, y, (255 *  meanMiddleBrighness) + (corrY) );
        }
        else
        {
          getImage().shadingCorrection.setY(x, y, 255 *  meanMiddleBrighness);
        }
      }
    }
  }
  DEBUG_REQUEST("Image:Corrector:GaussianBlur",
    if(correctionCycle == 1)
    {
      cv::Mat matYc(getImage().height(), getImage().width(), CV_16U, getImage().shadingCorrection.getYcPointer());
      unsigned short* blurredYc = new unsigned short[getImage().shadingCorrection.getSize()];
      cv::Mat matYcBlurred(getImage().height(), getImage().width(), CV_16U, blurredYc);

      cv::Size size(5, 5);
      cv::GaussianBlur(matYc, matYcBlurred, size, 10, 10);
      memcpy(matYc.data, matYcBlurred.data, getImage().shadingCorrection.getSize() * sizeof(unsigned short));
    }
  );
}
