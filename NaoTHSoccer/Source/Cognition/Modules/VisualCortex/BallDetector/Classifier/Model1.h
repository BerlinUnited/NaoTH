#ifndef _CNN_H
#define _CNN_H

# include <emmintrin.h>
# include <math.h>

#include "AbstractCNNClassifier.h"

class Model1 : public AbstractCNNFinder {

public:
	void cnn(float x0[16][16][1]);
	void find(const BallCandidates::PatchYUVClassified& p,double meanBrightness);
	virtual double getRadius();
	virtual Vector2d getCenter();
	virtual double getBallConfidence();

private:
	float in_step[16][16][1];
	float scores[4];

};
# endif
