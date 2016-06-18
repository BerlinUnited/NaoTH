/**
* @file CVHaarClassifier.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _CVHaarClassifier_H_
#define _CVHaarClassifier_H_

#include "Tools/naoth_opencv.h"
#include "Representations/Perception/BallCandidates.h"

class CVHaarClassifier
{
private:
  cv::CascadeClassifier cascadeClasifier;

public:
  CVHaarClassifier() {
    cascadeClasifier.load("Config/model_haar_cascade.xml");
  }


  bool classify(BallCandidates::Patch& p, naoth::CameraInfo::CameraID cameraId)
  {
    
    // TODO: magic numbers
    cv::Mat wrappedImg(12, 12, CV_8U, (void*) p.data.data());
    //cv::transpose(wrappedImg, wrappedImg);
    std::vector<cv::Rect> out;
    cascadeClasifier.detectMultiScale( wrappedImg, out, 1.01, 0, 0|CV_HAAR_SCALE_IMAGE, cv::Size(12,12) );

    return !out.empty();
  }
};

#endif // _CVHaarClassifier_H_
