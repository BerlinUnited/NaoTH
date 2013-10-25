#include "OpenCVImage.h"
#include <Tools/Debug/Stopwatch.h>

#include <vector>

OpenCVImage::OpenCVImage()
{
}

OpenCVImage::OpenCVImage(const Image &orig)
{
  image = convertFromNaoImage(orig);
}

cv::Mat OpenCVImage::convertFromNaoImage(const Image &orig, cv::Mat reuseMatrix)
{
  // construct a matrix that has an column for each color value of each pixel
  cv::Mat newImg;
  int oldType = reuseMatrix.type();

  STOPWATCH_START("OpenCVImage::convert::createMatrix");

  if((unsigned int) reuseMatrix.rows == orig.height()
     && (unsigned int) reuseMatrix.cols == orig.width()
     && oldType == CV_8UC(3))
  {
    // reuse the old one by reshaping it (only changes metadata)
    newImg = reuseMatrix.reshape(6);
  }
  else
  {
    // create a completly new one
    newImg = cv::Mat::zeros(orig.height(), orig.width()/2, CV_8UC(6));
  }
  STOPWATCH_STOP("OpenCVImage::convert::createMatrix");

  STOPWATCH_START("OpenCVImage::convert::wrap");
  // wrap the original one in a way that uses 4 columns for two pixels
  cv::Mat wrappedYUV422((int) orig.height(), (int) orig.width()/2,
                        CV_8UC4,
                        (void*) orig.yuv422);
  STOPWATCH_STOP("OpenCVImage::convert::wrap");


  STOPWATCH_START("OpenCVImage::convert::mixChannels");
  std::vector<cv::Mat> src(1);
  src[0] = wrappedYUV422;

  std::vector<cv::Mat> dst(1);
  dst[0] = newImg;

  // Y1 U Y12 V will be converted to Y1 U V, Y2 U V
  // Y1=0 -> 0
  // U=1 -> 1,4
  // Y2=2 -> 3
  // V=3 -> 2,5
  int fromTo[] = { 0,0, 1,1, 1,4, 2,3, 3,2, 3,5};

  cv::mixChannels(src, dst, fromTo, 6);
  STOPWATCH_STOP("OpenCVImage::convert::mixChannels");

  // we now have a real matrix with correct color values for each pixel,
  // reshape to a true YUV-color model to make the life easier for users
  // of this matrix
  return newImg.reshape(3);
}

cv::Mat OpenCVImage::convertGrayscaleFromNaoImage(const Image &orig, cv::Mat reuseMatrix)
{
  cv::Mat wrappedYUV422((int) orig.height(), (int) orig.width(),
                        CV_8UC2,
                        (void*) orig.yuv422);
  return wrappedYUV422;
}
