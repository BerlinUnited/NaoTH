#ifndef MBC_36KSM_FINETUNED_H
#define MBC_36KSM_FINETUNED_H

#include "AbstractCNNClassifier.h"

class mbc_36ksm_finetuned : public AbstractCNNFinder {

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
