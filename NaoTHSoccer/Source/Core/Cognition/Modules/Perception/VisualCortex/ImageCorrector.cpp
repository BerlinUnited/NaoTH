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
  DEBUG_REQUEST_REGISTER("Vision:ImageCorrector:correctBrighnessInImage", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ImageCorrector:simpleBrighnessCorrection", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ImageCorrector:GaussianBlur", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ImageCorrector:reset", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ImageCorrector:saveToFile", " ", false);
  DEBUG_REQUEST_REGISTER("Vision:ImageCorrector:loadFromFile", " ", false);

  shadingCorrection.init(getImage().width(), getImage().height(), getImage().cameraInfo.cameraID);
}

ImageCorrector::~ImageCorrector()
{
}

void ImageCorrector::execute()
{
  simpleBrighnessCorrectionRequestActive = false;
  DEBUG_REQUEST("Vision:ImageCorrector:simpleBrighnessCorrection",
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

  DEBUG_REQUEST("Vision:ImageCorrector:reset",
    shadingCorrection.reset();
  );

  DEBUG_REQUEST("Vision:ImageCorrector:correctBrighnessInImage",
    Pixel pixel;
    for(unsigned int y = 0; y < getImage().height(); y++)
    {
      for(unsigned int x = 0; x < getImage().width(); x++)
      {
        PixelT<unsigned short> newPixel;
        getImage().get(x, y, pixel);
        newPixel.y = static_cast<unsigned short>((shadingCorrection.get(0, x, y) * pixel.y) >> 10);
        newPixel.u = static_cast<unsigned short>((shadingCorrection.get(1, x, y) * pixel.u) >> 10);
        newPixel.v = static_cast<unsigned short>((shadingCorrection.get(2, x, y) * pixel.v) >> 10);
       
        pixel.y = (unsigned char) Math::clamp<unsigned short>(newPixel.y, 0, 255);
        pixel.u = (unsigned char) Math::clamp<unsigned short>(newPixel.u, 0, 255);
        pixel.v = (unsigned char) Math::clamp<unsigned short>(newPixel.v, 0, 255);

        getImage().set(x, y, pixel);
      }
    }
  );

  DEBUG_REQUEST("Vision:ImageCorrector:saveToFile",
    shadingCorrection.saveCorrectionToFiles
    (
      Platform::getInstance().theConfigDirectory,
      Platform::getInstance().theHeadHardwareIdentity
    );
  );

  DEBUG_REQUEST("Vision:ImageCorrector:loadFromFile",
    shadingCorrection.loadCorrectionFromFiles
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

  Pixel pix1;
  getImage().get(x1,y1, pix1);
  Pixel pix2;
  getImage().get(x1,y2, pix2);
  Pixel pix3;
  getImage().get(x2,y1, pix3);
  Pixel pix4;
  getImage().get(x2,y2, pix4);

  PixelT<unsigned short> meanMiddle;

  meanMiddle.y = static_cast<unsigned short>(pix1.y + pix2.y + pix3.y + pix4.y);
  meanMiddle.u = static_cast<unsigned short>(pix1.u + pix2.u + pix3.u + pix4.u);
  meanMiddle.v = static_cast<unsigned short>(pix1.v + pix2.v + pix3.v + pix4.v);

  Pixel pixel;

  for(unsigned int y = 0; y < getImage().height(); y++)
  {
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      getImage().get(x, y, pixel);
      PixelT<unsigned short> corr;
      corr.y = static_cast<unsigned short>(shadingCorrection.get(0, x, y));
      corr.u = static_cast<unsigned short>(shadingCorrection.get(1, x, y));
      corr.v = static_cast<unsigned short>(shadingCorrection.get(2, x, y));

      if(correctionCycle != 255)
      {
        if(pixel.y > 0)
        {
          shadingCorrection.set(0, x, y, static_cast<unsigned short>(((255 *  meanMiddle.y / pixel.y) + corr.y) / 2) );
        }
        else if(pixel.y == 0)
        {
          shadingCorrection.set(0, x, y, static_cast<unsigned short>((255 *  meanMiddle.y) + corr.y) );
        }
        if(pixel.u > 0)
        {
          shadingCorrection.set(1, x, y, static_cast<unsigned short>(((255 *  meanMiddle.u / pixel.u) + corr.u) / 2) );
        }
        else if(pixel.u == 0)
        {
          shadingCorrection.set(1, x, y, static_cast<unsigned short>((255 *  meanMiddle.u) + corr.u));
        }
        if(pixel.v > 0)
        {
          shadingCorrection.set(2, x, y, static_cast<unsigned short>(((255 *  meanMiddle.v / pixel.v) + corr.v) / 2) );
        }
        else if(pixel.v == 0)
        {
          shadingCorrection.set(2, x, y, static_cast<unsigned short>((255 *  meanMiddle.v) + corr.v));
        }
      }
      else
      {
        if(pixel.y > 0)
        {
          shadingCorrection.set(0, x, y, static_cast<unsigned short>(255 *  meanMiddle.y / pixel.y));
        }
        else if(pixel.y == 0)
        {
          shadingCorrection.set(0, x, y, static_cast<unsigned short>(255 *  meanMiddle.y));
        }
        if(pixel.u > 0)
        {
          shadingCorrection.set(1, x, y, static_cast<unsigned short>(255 *  meanMiddle.u / pixel.u));
        }
        else if(pixel.u == 0)
        {
          shadingCorrection.set(1, x, y, static_cast<unsigned short>(255 *  meanMiddle.u));
        }
        if(pixel.v > 0)
        {
          shadingCorrection.set(2, x, y, static_cast<unsigned short>(255 *  meanMiddle.v / pixel.v));
        }
        else if(pixel.v == 0)
        {
          shadingCorrection.set(2, x, y, static_cast<unsigned short>(255 *  meanMiddle.v));
        }
      }
    }
  }
  DEBUG_REQUEST("Vision:ImageCorrector:GaussianBlur",
    if(correctionCycle == 1)
    {
      cv::Size size(5, 5);

      cv::Mat matYc(getImage().height(), getImage().width(), CV_16U, shadingCorrection.getDataPointer(0));
      unsigned short* blurredYc = new unsigned short[shadingCorrection.getSize()];
      cv::Mat matYcBlurred(getImage().height(), getImage().width(), CV_16U, blurredYc);
      cv::GaussianBlur(matYc, matYcBlurred, size, 10, 10);
      memcpy(matYc.data, matYcBlurred.data, shadingCorrection.getSize() * sizeof(unsigned short));
      delete blurredYc;

      cv::Mat matUc(getImage().height(), getImage().width(), CV_16U, shadingCorrection.getDataPointer(1));
      unsigned short* blurredUc = new unsigned short[shadingCorrection.getSize()];
      cv::Mat matUcBlurred(getImage().height(), getImage().width(), CV_16U, blurredUc);
      cv::GaussianBlur(matUc, matUcBlurred, size, 10, 10);
      memcpy(matUc.data, matUcBlurred.data, shadingCorrection.getSize() * sizeof(unsigned short));
      delete blurredUc;

      cv::Mat matVc(getImage().height(), getImage().width(), CV_16U, shadingCorrection.getDataPointer(2));
      unsigned short* blurredVc = new unsigned short[shadingCorrection.getSize()];
      cv::Mat matVcBlurred(getImage().height(), getImage().width(), CV_16U, blurredVc);
      cv::GaussianBlur(matVc, matVcBlurred, size, 10, 10);
      memcpy(matVc.data, matVcBlurred.data, shadingCorrection.getSize() * sizeof(unsigned short));
      delete blurredVc;
    }
  );
}
