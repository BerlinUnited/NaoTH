#ifndef _FY1500_CONF_H
#define _FY1500_CONF_H

# include <emmintrin.h>

#include "AbstractCNNClassifier.h"

class Fy1500_Conf : public AbstractCNNFinder {

public:
	void cnn(float x0[16][16][1]);
	void predict(const BallCandidates::PatchYUVClassified& p,double meanBrightness);
	virtual double getRadius() const;
	virtual Vector2d getCenter() const;
	virtual double getBallConfidence() const;

private:
	float in_step[16][16][1];
	float scores[4];

};
# endif
