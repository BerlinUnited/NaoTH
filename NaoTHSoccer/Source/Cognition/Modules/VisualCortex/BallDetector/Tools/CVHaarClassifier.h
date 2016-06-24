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
  cv::Mat buffer;
  int idx;

public:
  CVHaarClassifier() : buffer(32, 32, CV_8UC1, cv::Scalar(128)) {
    cascadeClasifier.load("Config/model_haar_cascade.xml");
    idx = 0;
  }


  int classify(BallCandidates::Patch& p)
  {
    ASSERT(p.data.size() == 12*12);
    // TODO: magic numbers
    cv::Mat wrappedImg(12, 12, CV_8U, (void*) p.data.data());
    cv::transpose(wrappedImg, wrappedImg);

    //cv::Mat aux = buffer.colRange(40,44).rowRange(40,44);
    //wrappedImg.copyTo(aux);

    cv::Mat aux(buffer(cv::Rect(10,10,12,12)));
    wrappedImg.copyTo(aux);

    std::vector<cv::Rect> out;
    // TODO: more magic numbers
    //std::stringstream ss;
    //ss << "p-" << idx << ".png";
    //cv::imwrite(ss.str(), buffer);
    
    cascadeClasifier.detectMultiScale( buffer, out, 1.01, 0, 0|CV_HAAR_SCALE_IMAGE, cv::Size(8,8), cv::Size(12,12) );
    
    idx++;
    return out.size();
  }
};

#endif // _CVHaarClassifier_H_
