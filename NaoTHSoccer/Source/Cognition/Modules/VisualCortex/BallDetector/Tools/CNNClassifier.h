#ifndef _CNNClassifier_H_
#define _CNNClassifier_H_

#include <limits>

#include "Representations/Perception/BallCandidates.h"

class CNNClassifier{

public:
	CNNClassifier(){}

	bool classify(const BallCandidates::Patch& p);

	// declare copied intial input array
	double out_step0[16][16][1];

	// declare output for this image input step
	double out_step1[16][16][1];

	// declare output for this convolution step
	double out_step2[7][7][8];

	// declare output for this ReLU step
	double out_step3[7][7][8];

	// declare output for this convolution step
	double out_step4[3][3][10];

	// declare output for this ReLU step
	double out_step5[3][3][10];

	// declare output for this fully connected step
	double out_step6[2][1];

	// declare output for this SoftMax step
	double out_step7[2][1];

};

#endif

