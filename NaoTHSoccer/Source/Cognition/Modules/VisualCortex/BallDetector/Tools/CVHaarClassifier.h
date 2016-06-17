/**
* @file CVHaarClassifier.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _CVHaarClassifier_H_
#define _CVHaarClassifier_H_

#include "Tools/naoth_opencv.h"
#include "Representations/Perception/BallCandidates.h"
#include "Representations/Infrastructure/CameraInfo.h"

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
    cv::Mat wrappedImg(12, 12, CV_8UC1, (void*) p.data.data());
    std::vector<Rect> out;
    face_cascade.detectMultiScale( wrappedImg, out, 1.1, 2, 0|cv::CV_HAAR_SCALE_IMAGE, Size(12,12) );

    return !out.empty();
  }
};

#endif // _CVHaarClassifier_H_
