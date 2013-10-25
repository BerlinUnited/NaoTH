#include "OpenCVImageProvider.h"

OpenCVImageProvider::OpenCVImageProvider()
{
}

void OpenCVImageProvider::execute()
{
  const Image& image = getImage();
  const Image& imageTop = getImageTop();

  getCVImage().image = CVImage::convertFromNaoImage(image, getCVImage().image);
  getCVImageTop().image = CVImage::convertFromNaoImage(imageTop, getCVImageTop().image);
  //getCVImage().image = CVImage::convertGrayscaleFromNaoImage(
  //      image, getCVImage().image);

  // debug
//  cv::Mat asRgb;
//  cv::cvtColor(getCVImage().image, asRgb, CV_YCrCb2RGB);
//  cv::namedWindow("cv", cv::WINDOW_AUTOSIZE);
//  cv::imshow("cv",asRgb);
//  cv::waitKey(1);

}

OpenCVImageProvider::~OpenCVImageProvider()
{
}
