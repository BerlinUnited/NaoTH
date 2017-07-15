/**
* @file CVHaarClassifier.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _CVHaarClassifier_H_
#define _CVHaarClassifier_H_

#include "Tools/naoth_opencv.h"
#include "../Classifier/AbstractCNNClassifier.h" // HACK!

#include "Representations/Perception/BallCandidates.h"

class CVHaarClassifier : public AbstractCNNClassifier // HACK!
{
private:
  cv::CascadeClassifier cascadeClasifier;
  cv::Mat buffer;
  int idx;
  std::vector<cv::Rect> out;

public:
  CVHaarClassifier() : buffer(24, 24, CV_8UC1, cv::Scalar(128)) {
    //setLoadModel("experiment1.xml");
    idx = 0;
  }

  CVHaarClassifier(const std::string& name) : buffer(24, 24, CV_8UC1, cv::Scalar(128)) {
    //setLoadModel("experiment1.xml");
    idx = 0;

    loadModel(name);

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

  // only defined because of inheritance
  bool classify(const BallCandidates::Patch &p){
    return classify(p, 0, 18) > 0;
  }

  // overload classify from AbstractCNNClassifier
  int classify(const BallCandidates::Patch& p, unsigned int minNeighbours=0, unsigned int windowSize=12)
  {
    out.clear();

    ASSERT(p.data.size() == 16*16);
    // TODO: magic numbers
    cv::Mat wrappedImg(16, 16, CV_8U, (void*) p.data.data());
    cv::transpose(wrappedImg, wrappedImg);

    

    //cv::Mat aux = buffer.colRange(40,44).rowRange(40,44);
    //wrappedImg.copyTo(aux);

    cv::Mat aux(buffer(cv::Rect(4,4,16,16)));
    //wrappedImg.copyTo(aux);

    cv::GaussianBlur(wrappedImg, aux, cv::Size(3,3), 0, 0);

   
    //std::vector<int> rejectLevels;
    //std::vector<double> levelWeights;
    // TODO: more magic numbers
    //std::stringstream ss;
    //ss << "p-" << idx << ".png";
    //cv::imwrite(ss.str(), buffer);
    
    //cascadeClasifier.detectMultiScale( buffer, out, rejectLevels, levelWeights, 1.05, minNeighbours, 0|CV_HAAR_SCALE_IMAGE, cv::Size(10,10), cv::Size(windowSize,windowSize) , true);
    cascadeClasifier.detectMultiScale( buffer, out, 1.05, minNeighbours, 0|CV_HAAR_SCALE_IMAGE, cv::Size(13,13), cv::Size(windowSize,windowSize));

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
