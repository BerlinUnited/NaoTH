#ifndef FY_1500_OLD_TK03_COMBINED_DETECTION_NNCG_H
#define FY_1500_OLD_TK03_COMBINED_DETECTION_NNCG_H

#include <emmintrin.h>

#include "AbstractCNNClassifier.h"

class fy_1500_old_tk03_combined_detection_nncg : public AbstractCNNFinder {

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
