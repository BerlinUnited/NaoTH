#include "OpenCVImage.h"

OpenCVImage::OpenCVImage()
{
}

OpenCVImage::OpenCVImage(const Image &orig)
{
  data = convertFromNaoImage(orig);
}

cv::Mat OpenCVImage::convertFromNaoImage(const Image &orig)
{
  return cv::Mat::ones(1,1, CV_8UC1);
}
