#ifndef _CNN_dortmund2018_keras_H_
#define _CNN_dortmund2018_keras_H_

#include <limits>

#include <fstream>

#include "../AbstractCNNClassifier.h"

class CNN_dortmund2018_keras : public AbstractCNNClassifier {

public:
  CNN_dortmund2018_keras();

  ~CNN_dortmund2018_keras(){}

	bool classify(const BallCandidates::Patch& p);
  bool classify(const BallCandidates::PatchYUVClassified& p);
  virtual float getBallConfidence();
  virtual float getNoballConfidence();

private:

  float in_step[16][16][1];

  int res;
  float scores[2];


private:
	int cnn(float x0[16][16][1]);
};

#endif // _CNN_dortmund2018_keras_H_

