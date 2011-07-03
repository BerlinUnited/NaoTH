#include "Video2Log.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

#include <Tools/Debug/NaoTHAssert.h>
#include <Representations/Infrastructure/Image.h>
#include <Core/Tools/Logfile/LogfileManager.h>
#include <Tools/DataStructures/Serializer.h>

Video2Log::Video2Log(std::string fileName, std::string outFile)
  : path(fileName), outFile(outFile)
{

}

void Video2Log::generateLogFromVideoFile()
{
  cv::VideoCapture capture(path);
  LogfileManager<10> mgr(true);
  mgr.openFile(outFile.c_str());

  unsigned int frameNumber = 0;
  while(capture.isOpened())
  {
    cv::Mat matImgOrig;
    cv::Mat matImg;
    capture >> matImgOrig;

    ASSERT(matImgOrig.type() == CV_8SC3);

    if(matImgOrig.rows == 0)
    {
      break;
    }

    naoth::Image repImg;
    // always resize to 320x240
    ASSERT(repImg.width() == 320);
    ASSERT(repImg.height() == 240);

    if(matImgOrig.rows != repImg.height() || matImgOrig.cols != repImg.width())
    {
      cv::resize(matImgOrig, matImg, cv::Size(repImg.width(),repImg.height()));
    }
    else
    {
      matImg = matImgOrig;
    }
    ASSERT(matImg.rows == repImg.height());
    ASSERT(matImg.cols == repImg.width());

    // we need YCbCr color space
    cv::cvtColor(matImg, matImg, CV_RGB2YCrCb);

    // convert the classical way
    for(unsigned int x=0; x < matImg.cols && x < repImg.width(); x++)
    {
      for(unsigned int y=0; y < matImg.rows && y < repImg.height(); y++)
      {
        cv::Vec3b v = matImg.at<cv::Vec3b>(y,x);
        Pixel p;
        p.y = v[0];
        p.u = v[1];
        p.v = v[2];
        repImg.set(x, y, p);
      }
    }

    // write out
    std::stringstream& stream = mgr.log(frameNumber, "Image");
    naoth::Serializer<naoth::Image>::serialize(repImg, stream);

    frameNumber++;

  }
  mgr.flush();
  mgr.closeFile();
}

Video2Log::~Video2Log()
{

}

int main(int argc, char** argv)
{
  if(argc < 3)
  {
    std::cout << "usage: " << std::endl << "  video2log <videofile> <outfile>" << std::endl;
  }
  else
  {
    std::string fileName(argv[1]);
    std::string outFile(argv[2]);

    std::cout << "opening \"" << fileName << "\"" << std::endl;
    Video2Log converter(fileName, outFile);
    converter.generateLogFromVideoFile();
  }
  return 0;
}
