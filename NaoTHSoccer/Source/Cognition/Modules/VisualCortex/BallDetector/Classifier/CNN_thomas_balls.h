#ifndef _CNN_THOMAS_BALLS_H
#define _CNN_THOMAS_BALLS_H

# include <emmintrin.h>
# include <math.h>

#include "AbstractCNNClassifier.h"

class CNN_THOMAS_BALLS : public AbstractCNNClassifier {

public:
	int cnn(float x0[16][16][1]);
	bool classify(const BallCandidates::Patch& p);
	bool classify(const BallCandidates::PatchYUVClassified& p);
	virtual float getBallConfidence();
	virtual float getNoballConfidence();

private:
	float in_step[16][16][1];
	int res[1];
	double scores[2];

};
# endif
