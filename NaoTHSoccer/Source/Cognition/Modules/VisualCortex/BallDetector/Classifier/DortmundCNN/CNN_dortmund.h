#ifndef _CNN_dortmund_H_
#define _CNN_dortmund_H_

#include <limits>

#include <fstream>

#include "../AbstractCNNClassifier.h"

class CNN_dortmund : public AbstractCNNClassifier {

public:
  CNN_dortmund();

  ~CNN_dortmund(){}

	bool classify(const BallCandidates::Patch& p);
  virtual float getBallConfidence();
  virtual float getNoballConfidence();

private:

  float in_step[16][16][1];

  int res[1];
  float scores[2];


private:
  int cnn(float x0[16][16][1]);
};

#endif

