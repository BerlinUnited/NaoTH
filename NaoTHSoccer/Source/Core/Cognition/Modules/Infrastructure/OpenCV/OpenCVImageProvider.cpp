#include "OpenCVImageProvider.h"

OpenCVImageProvider::OpenCVImageProvider()
{
}

void OpenCVImageProvider::execute()
{
  const Image& image = getImage();
  if(image.cameraInfo.resolutionWidth == 320 && image.cameraInfo.resolutionHeight == 240)
  {
    OpenCVGrayScale::createSmallGrayScaleFromRaw(image.yuv422, getOpenCVGrayScale().image);
  }

  getOpenCVImage().data = OpenCVImage::convertFromNaoImage(image);

}

OpenCVImageProvider::~OpenCVImageProvider()
{
}
