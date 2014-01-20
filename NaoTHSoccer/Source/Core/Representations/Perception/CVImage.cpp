#include "CVImage.h"
#include <Tools/Debug/Stopwatch.h>

#include <vector>

CVImage::CVImage()
{
}

CVImage::CVImage(const Image &orig)
{
  image = convertFromNaoImage(orig);
}

cv::Mat CVImage::convertFromNaoImage(const Image &orig, cv::Mat result)
{
  // reuse the old one by reshaping it (only changes metadata) or create a new object
  if(result.rows == (int)orig.height() && result.cols == (int)orig.width() && result.type() == CV_8UC3) {
    result = result.reshape(6);
  } else {
    result = cv::Mat::zeros(orig.height(), orig.width()/2, CV_8UC(6));
  }

  // wrap the original one in a way that uses 4 columns for two pixels
  cv::Mat wrappedYUV422((int) orig.height(), (int) orig.width()/2,
                        CV_8UC4, 
                        (void*) orig.yuv422);

  // Y1 U Y12 V will be converted to Y1 U V, Y2 U V
  // Y1=0 -> 0
  // U=1 -> 1,4
  // Y2=2 -> 3
  // V=3 -> 2,5
  const int fromTo[] = { 0,0, 1,1, 1,4, 2,3, 3,2, 3,5 };
  cv::mixChannels(&wrappedYUV422, 1, &result, 1, fromTo, 6);

  // we now have a real matrix with correct color values for each pixel,
  // reshape to a true YUV-color model to make the life easier for users
  // of this matrix
  return result.reshape(3);
}

cv::Mat CVImage::convertGrayscaleFromNaoImage(const Image &orig, cv::Mat result)
{
  // reuse the old one by reshaping it (only changes metadata) or create a new object
  if(result.rows != (int)orig.height() || result.cols != (int)orig.width() || result.type() != CV_8U) {
    result = cv::Mat::zeros(orig.height(), orig.width(), CV_8U);
  }

  cv::Mat wrappedYUV422((int) orig.height(), (int) orig.width(),
                        CV_8UC2,
                        (void*) orig.yuv422);

  // copy only the Y channel
  const int fromTo[] = { 0,0 };
  cv::mixChannels(&wrappedYUV422, 1, &result, 1, fromTo, 1);

  return result;
}
