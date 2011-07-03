#include "Video2Log.h"

#include <glib.h>

#include <iostream>

#include <Tools/Debug/NaoTHAssert.h>
#include <Representations/Infrastructure/Image.h>
#include <Tools/DataStructures/Serializer.h>

Video2Log::Video2Log(std::string fileName, std::string outFile)
  : path(fileName), outFile(outFile), mgr(true), frameNumber(0)
{
  mgr.openFile(outFile.c_str());
}

void Video2Log::generateLogFromVideoFile()
{
  cv::VideoCapture capture(path);

  while(capture.isOpened())
  {
    cv::Mat matImgOrig;
    capture >> matImgOrig;

    if(matImgOrig.rows == 0)
    {
      break;
    }
    logImage(matImgOrig);
  }
}

void Video2Log::generateLogFromImageDir()
{
  GDir* dir = g_dir_open(path.c_str(), 0, NULL);
  if(dir != NULL)
  {
    const gchar* name;
    while ((name = g_dir_read_name(dir)) != NULL)
    {
      std::string completeFileName = path + "/" + name;
      if (g_file_test(completeFileName.c_str(), G_FILE_TEST_IS_REGULAR))
      {
        std::cout << "adding \"" << completeFileName << "\"" << std::endl;
        cv::Mat matImgOrig = cv::imread(completeFileName);
        logImage(matImgOrig);
      }
    }
    g_dir_close(dir);
  }
}

void Video2Log::logImage(cv::Mat &matImgOrig)
{
  cv::Mat matImg;
  naoth::Image repImg;


  ASSERT(matImgOrig.type() == CV_8SC3);

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

Video2Log::~Video2Log()
{
  mgr.flush();
  mgr.closeFile();
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
    if(g_file_test(fileName.c_str(), G_FILE_TEST_IS_DIR))
    {
      converter.generateLogFromImageDir();
    }
    else
    {
      converter.generateLogFromVideoFile();
    }
  }
  return 0;
}
