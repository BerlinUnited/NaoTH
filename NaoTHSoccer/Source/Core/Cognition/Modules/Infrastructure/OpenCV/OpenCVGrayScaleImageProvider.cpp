#include "OpenCVGrayScaleImageProvider.h"

OpenCVGrayScaleImageProvider::OpenCVGrayScaleImageProvider()
{
}

void OpenCVGrayScaleImageProvider::execute()
{
  const Image& image = getImage();
  const Image& imageTop = getImageTop();

  getCVImage().image = CVImage::convertGrayscaleFromNaoImage(
        image, getCVImage().image);
  getCVImageTop().image = CVImage::convertGrayscaleFromNaoImage(
        imageTop, getCVImageTop().image);

}
