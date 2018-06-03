#ifndef _CNN_dortmund2018_H_
#define _CNN_dortmund2018_H_

#include <limits>

#include <fstream>

#include "../AbstractCNNClassifier.h"

class CNN_dortmund2018 : public AbstractCNNClassifier {

public:
  CNN_dortmund2018();

  ~CNN_dortmund2018(){}

	bool classify(const BallCandidates::Patch& p);
  virtual float getBallConfidence();
  virtual float getNoballConfidence();

private:

  float in_step[16][16][1];

  int res[1];
  float scores[2];


private:
	int test(float x0[16][16][1]);
	int cnn(float x0[16][16][1]);
};

#endif // _CNN_dortmund2018_H_

