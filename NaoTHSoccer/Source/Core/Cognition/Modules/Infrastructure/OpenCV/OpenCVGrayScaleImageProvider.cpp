#include "OpenCVGrayScaleImageProvider.h"

OpenCVGrayScaleImageProvider::OpenCVGrayScaleImageProvider()
{
}

void OpenCVGrayScaleImageProvider::execute()
{
  getCVImage().image = CVImage::convertGrayscaleFromNaoImage(
        getImage(), getCVImage().image);
  getCVImageTop().image = CVImage::convertGrayscaleFromNaoImage(
        getImageTop(), getCVImageTop().image);

}
