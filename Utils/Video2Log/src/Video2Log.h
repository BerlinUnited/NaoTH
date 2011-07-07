#ifndef VIDEO2LOG_H
#define VIDEO2LOG_H

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <Core/Tools/Logfile/LogfileManager.h>

class Video2Log
{
public:
    Video2Log(std::string fileName, std::string outFile);
    virtual void generateLogFromVideoFile();
    virtual void generateLogFromImageDir();

    virtual ~Video2Log();
private:
    std::string path;
    std::string outFile;
    LogfileManager<10> mgr;
    unsigned int frameNumber;

    virtual void logImage(cv::Mat& matImgOrig);
};

#endif // VIDEO2LOG_H
