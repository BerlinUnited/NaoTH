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
  CVHaarClassifier() : buffer(24, 24, CV_8UC1, cv::Scalar(128)) {
    //setLoadModel("experiment1.xml");
    idx = 0;
  }

  bool modelLoaded() const {
    return !cascadeClasifier.empty();
  }

  void loadModel(const std::string& name) {
    std::stringstream ss;
    ss << "Config/modelHaarCascade/" << name;
    cascadeClasifier.load(ss.str());
    ASSERT(modelLoaded());
  }

  int classify(const BallCandidates::Patch& p, unsigned int minNeighbours=0, unsigned int windowSize=12)
  {
    cv::Mat img;
    if(p.data.size() == 12*12)
    {
      cv::Mat wrappedImg(12, 12, CV_8U, (void*) p.data.data());
      img = wrappedImg; // only copies the header, not the image itself;
    }
    else if(p.data.size() == 24*24)
    {
      cv::Mat wrappedImg(24, 24, CV_8U, (void*) p.data.data());
      cv::resize(wrappedImg, img, cv::Size(12,12));
    }
    else if(p.data.size() == 36*36)
    {
      cv::Mat wrappedImg(36, 36, CV_8U, (void*) p.data.data());
      cv::resize(wrappedImg, img, cv::Size(12,12));
    }
    else
    {
      ASSERT(p.data.size() == 12*12 || p.data.size() == 24*24 || p.data.size() == 36*36);
    }
    // TODO: magic numbers
    cv::transpose(img, img);
    //cv::flip(img,img, 1);

    //cv::Mat aux = buffer.colRange(40,44).rowRange(40,44);
    //wrappedImg.copyTo(aux);

    cv::Mat aux(buffer(cv::Rect(6,6,12,12)));
    //wrappedImg.copyTo(aux);

    cv::GaussianBlur(img, aux, cv::Size(3,3), 0, 0);

    std::vector<cv::Rect> out;
    std::vector<int> rejectLevels;
    std::vector<double> levelWeights;
    // TODO: more magic numbers
    //std::stringstream ss;
    //ss << "p-" << idx << ".png";
    //cv::imwrite(ss.str(), buffer);
    
    //cascadeClasifier.detectMultiScale( buffer, out, rejectLevels, levelWeights, 1.05, minNeighbours, 0|CV_HAAR_SCALE_IMAGE, cv::Size(10,10), cv::Size(windowSize,windowSize) , true);
    cascadeClasifier.detectMultiScale( buffer, out, 1.05, minNeighbours, 0|CV_HAAR_SCALE_IMAGE, cv::Size(10,10), cv::Size(windowSize,windowSize));

    idx++;

    /*
    for(size_t i = 0; i < levelWeights.size(); i++) {
      if(levelWeights[i] > 0) {return true;}
    }
    return false;
    */
    
    return static_cast<int>(out.size());
  }
};

#endif // _CVHaarClassifier_H_
