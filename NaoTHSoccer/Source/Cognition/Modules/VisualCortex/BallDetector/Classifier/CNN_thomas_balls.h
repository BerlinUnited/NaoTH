#ifndef _CNN_THOMAS_BALLS_H
#define _CNN_THOMAS_BALLS_H

# include <emmintrin.h>
# include <math.h>

#include "AbstractCNNClassifier.h"

class CNN_THOMAS_BALLS : public AbstractCNNFinder {

public:
	void cnn(float x0[16][16][1]);
	void find(const BallCandidates::PatchYUVClassified& p, double meanBrightness);

	double getRadius();
    Vector2d getCenter();

private:
	float in_step[16][16][1];
	double scores[3];

};
# endif
