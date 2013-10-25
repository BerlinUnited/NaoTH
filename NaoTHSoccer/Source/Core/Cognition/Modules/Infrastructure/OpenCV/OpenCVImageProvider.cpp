#include "OpenCVImageProvider.h"

OpenCVImageProvider::OpenCVImageProvider()
{
}

void OpenCVImageProvider::execute()
{
  const Image& image = getImage();

  getOpenCVImage().image = OpenCVImage::convertFromNaoImage(image, getOpenCVImage().image);

  // debug
//  cv::Mat asRgb;
//  cv::cvtColor(getOpenCVImage().image, asRgb, CV_YCrCb2RGB);
//  cv::namedWindow("cv", cv::WINDOW_AUTOSIZE);
//  cv::imshow("cv",asRgb);
//  cv::waitKey(1);

}

OpenCVImageProvider::~OpenCVImageProvider()
{
}
