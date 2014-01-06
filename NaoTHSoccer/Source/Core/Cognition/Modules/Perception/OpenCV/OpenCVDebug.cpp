#include "OpenCVDebug.h"
#include "Tools/Debug/DebugRequest.h"

OpenCVDebug::OpenCVDebug()
{
  DEBUG_REQUEST_REGISTER("OpenCVDebug:show_debug_image", "overwrite the original image with the debug image", false);
}

void OpenCVDebug::execute()
{
  Image& theImage = getImage();

  DEBUG_REQUEST("OpenCVDebug:show_debug_image",
  // write result to real image (for debugging...)
  cv::Mat result = getOpenCVGrayScale().image;
  
  for(unsigned int x=0; x < theImage.width(); x++) {
    for(unsigned int y=0; y < theImage.height(); y++)
  {
    if((int)x < result.cols && (int)y < result.rows)
    {
      Pixel p;

      if(result.channels() == 3)
      {
        cv::Vec3b& value = result.at<cv::Vec3b>(y,x);
        p.y = value[0];
        p.u = value[1];
        p.v = value[2];
      }
      else if(result.channels() == 4)
      {
        cv::Vec4b& value = result.at<cv::Vec4b>(y,x);
        p.y = value[0];
        p.u = value[1];
        p.v = value[3];
      }
      else if(result.channels() == 1)
      {
        uchar value = result.at<uchar>(y,x);
        p.y = value;
        p.u = 128;
        p.v = 128;
      }
      else
      {
        p.y = 0;
        p.u = 0;
        p.v = 0;
      }
      theImage.set(x,y,p);
    }
    else
    {
      Pixel p;
      p.y = 0;
      p.u = 128;
      p.v = 128;
      theImage.set(x,y,p);
    }
  }} //end for x/y
  );

}
