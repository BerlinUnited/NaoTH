#ifndef _Model1_H
#define _Model1_H

# include <emmintrin.h>

#include "AbstractCNNClassifier.h"

class Model1 : public AbstractCNNFinder {

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
# endif // _Model1_H
