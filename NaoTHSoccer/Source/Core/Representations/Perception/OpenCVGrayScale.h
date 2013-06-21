#ifndef OPENCVGRAYSCALE_H
#define OPENCVGRAYSCALE_H

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <opencv2/core/core.hpp>
#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic error "-Wconversion"
#endif

class OpenCVGrayScale
{
public:
  OpenCVGrayScale();
  virtual ~OpenCVGrayScale();

  cv::Mat image;

  /** Takes a 320x240 image and creates an OpenCV 160x120 gray scale image copy */
  inline static void createSmallGrayScaleFromRaw(unsigned char* yuv422, cv::Mat& result)
  {
    const int inputWidth=320;
    const int inputHeight=240;

    const size_t inputLineSize = inputWidth*2; // read complete line at once
    const size_t destBufferSize = inputLineSize / 4;

    char sourceBuffer[inputLineSize];
    char* ptrSourceBuffer = sourceBuffer;

    char* source = (char*) yuv422;
    char* dest = (char*) result.data;

    for(int i=0; i < inputHeight; ++i) // skip every second line
    {
      if(i % 2 == 0)
      {
        // copy line to local input buffer
        memcpy(sourceBuffer, source, inputLineSize);

        // copy channel to destination for this line
        ptrSourceBuffer = sourceBuffer;
        for(size_t j=0; j < destBufferSize; ++j)
        {
          *dest++ = *ptrSourceBuffer++;
          ptrSourceBuffer += 3; // skip one one chroma and a complete pixel
        }
      }
      source += inputLineSize;
    }
  }

private:

};

#endif // OPENCVGRAYSCALE_H
