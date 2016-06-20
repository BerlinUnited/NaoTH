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


  int classify(BallCandidates::Patch& p)
  {
    ASSERT(p.data.size() == 24*24);
    // TODO: magic numbers
    cv::Mat wrappedImg(24, 24, CV_8U, (void*) p.data.data());
    //cv::transpose(wrappedImg, wrappedImg);
    std::vector<cv::Rect> out;
    // TODO: more magic numbers
    cascadeClasifier.detectMultiScale( wrappedImg, out, 1.1, 0, 0|CV_HAAR_SCALE_IMAGE, cv::Size(10,10), cv::Size(12,12) );

    return out.size();
  }
};

#endif // _CVHaarClassifier_H_
