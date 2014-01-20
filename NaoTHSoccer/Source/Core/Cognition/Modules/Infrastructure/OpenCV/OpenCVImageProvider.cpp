#include "OpenCVImageProvider.h"

OpenCVImageProvider::OpenCVImageProvider()
{
}

void OpenCVImageProvider::execute()
{
  getCVImage().image = CVImage::convertFromNaoImage(getImage(), getCVImage().image);
  getCVImageTop().image = CVImage::convertFromNaoImage(getImageTop(), getCVImageTop().image);

// debug
//  cv::Mat asRgb;
//  cv::cvtColor(getCVImage().image, asRgb, CV_YCrCb2RGB);
//  cv::namedWindow("cv", cv::WINDOW_AUTOSIZE);
//  cv::imshow("cv",asRgb);
//  cv::waitKey(1);
}
