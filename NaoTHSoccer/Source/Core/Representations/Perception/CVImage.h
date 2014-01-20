#ifndef OPENCVIMAGE_H
#define OPENCVIMAGE_H

#include <Representations/Infrastructure/Image.h>
#include <Tools/naoth_opencv.h>

using namespace naoth;

/**
 * @brief The CVImage class contains the robots camera image as OpenCV matrix
 */
class CVImage
{
public:

  cv::Mat image;

  CVImage();

  CVImage(const Image& orig);

  virtual ~CVImage() {}

  static cv::Mat convertFromNaoImage(const Image& orig, cv::Mat reuseMatrix = cv::Mat());
  static cv::Mat convertGrayscaleFromNaoImage(const Image& orig, cv::Mat reuseMatrix = cv::Mat());
};

class CVImageTop : public CVImage
{
public:
  virtual ~CVImageTop() {}
};

#endif // OPENCVIMAGE_H
