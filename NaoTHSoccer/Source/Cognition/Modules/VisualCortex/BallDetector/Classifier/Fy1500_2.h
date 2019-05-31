#ifndef _FY1500_2_H
#define _FY1500_2_H

# include <emmintrin.h>
# include <math.h>

#include "AbstractCNNClassifier.h"

class Fy1500_2 : public AbstractCNNFinder {

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
