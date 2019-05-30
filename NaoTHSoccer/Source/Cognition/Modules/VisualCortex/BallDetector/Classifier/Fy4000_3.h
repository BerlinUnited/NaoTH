#ifndef _FY4000_3_H
#define _FY4000_3_H

# include <emmintrin.h>
# include <math.h>

#include "AbstractCNNClassifier.h"

class Fy4000_3 : public AbstractCNNFinder {

public:
	void cnn(float x0[16][16][1]);
	void find(const BallCandidates::PatchYUVClassified& p,double meanBrightness);
	virtual double getRadius();
	virtual Vector2d getCenter();

private:
	float in_step[16][16][1];
	float scores[3];

};
# endif
