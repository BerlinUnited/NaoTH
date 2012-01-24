#include "OpenCVGrayScale.h"

OpenCVGrayScale::OpenCVGrayScale()
{
  image = cv::Mat::zeros(120,160, CV_8UC1);
}

OpenCVGrayScale::~OpenCVGrayScale()
{
}
