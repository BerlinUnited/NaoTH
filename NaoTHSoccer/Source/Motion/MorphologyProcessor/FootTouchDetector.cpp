/**
* @file FootTouchDetector.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* detect if the foot touch the ground
*/

#include "FootTouchDetector.h"

using namespace std;

FootTouchDetector::FootTouchDetector()
  : fsrData(NULL)
{
}

FootTouchDetector::~FootTouchDetector()
{
}

void FootTouchDetector::init(const double* fsr, const string& cfg)
{
  fsrData = fsr;
  touchTrainSet.clear();
  unTouchTrainSet.clear();

  std::stringstream ss(cfg);
  ss >> theLinearClassifier;
}//end init

void FootTouchDetector::save(std::string& cfg) const
{
  std::stringstream ofs;
  ofs << theLinearClassifier;
  cfg = ofs.str();
}//end save

bool FootTouchDetector::isTouch() const 
{
  return theLinearClassifier.classify(fsrData) > 0;
}//end isTouch

void FootTouchDetector::calibrate(bool isTouch)
{
  Vector_n<double,4> data(fsrData);
  if (isTouch){
      touchTrainSet.push_back(data);
  }else{
      unTouchTrainSet.push_back(data);
  }

  theLinearClassifier.perceptionTrain(touchTrainSet, unTouchTrainSet);
//    theLinearClassifier.LMSTrain(touchTrainSet, 10000, unTouchTrainSet, -10000, 1e-8, 50);
}//end calibrate
