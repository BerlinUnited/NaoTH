#ifndef OPENCVIMAGE_H
#define OPENCVIMAGE_H

#include <Representations/Infrastructure/Image.h>
#include <Tools/naoth_opencv.h>

using namespace naoth;

class OpenCVImage
{
public:

  cv::Mat image;

  OpenCVImage();

  OpenCVImage(const Image& orig);

  static cv::Mat convertFromNaoImage(const Image& orig, cv::Mat reuseMatrix = cv::Mat());
  static cv::Mat convertGrayscaleFromNaoImage(const Image& orig, cv::Mat reuseMatrix = cv::Mat());
};

#endif // OPENCVIMAGE_H
