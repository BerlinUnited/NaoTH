#ifndef FY_1500_NEW2_NEW_DATASET_H
#define FY_1500_NEW2_NEW_DATASET_H

#include <emmintrin.h>

#include "AbstractCNNClassifier.h"

class fy_1500_new2_new_dataset : public AbstractCNNFinder {

public:
	void cnn(float x0[16][16][1]);
	void predict(const BallCandidates::PatchYUVClassified& p,double meanBrightness);
	virtual double getRadius();
	virtual Vector2d getCenter();
	virtual double getBallConfidence();

private:
	float in_step[16][16][1];
	float scores[4];

};
# endif
