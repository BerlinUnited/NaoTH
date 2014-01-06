#include "OpenCVImageProvider.h"

OpenCVImageProvider::OpenCVImageProvider()
{
}

void OpenCVImageProvider::execute()
{
  const Image& image = getImage();
  if(image.width() == 320 && image.height() == 240)
  {
    OpenCVGrayScale::createSmallGrayScaleFromRaw(image.data(), getOpenCVGrayScale().image);
  }
}

OpenCVImageProvider::~OpenCVImageProvider()
{
}
