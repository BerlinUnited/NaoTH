#ifndef MBC_GOPEN_56K_FT_H
#define MBC_GOPEN_56K_FT_H

#include <emmintrin.h>

#include "AbstractCNNClassifier.h"

class mbc_gopen_56k_ft : public AbstractCNNFinder {

public:
	void cnn(float x0[16][16][1]);
	void predict(const BallCandidates::PatchYUVClassified& p,double meanBrightness);
	virtual double getRadius() const;
	virtual Vector2d getCenter() const;
	virtual double getBallConfidence() const;

private:
	float in_step[16][16][1];
	float scores[2];

};
# endif
