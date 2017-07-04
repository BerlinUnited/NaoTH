#ifndef _CNNClassifier_H_
#define _CNNClassifier_H_

#include <limits>

#include <fstream>

#include "Representations/Perception/BallCandidates.h"

class CNNClassifier{

public:
	CNNClassifier(){out.open("example.txt");}

	~CNNClassifier(){out.close();}

std::ofstream out;
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
	double out_step6[64][1][1];

	// declare output for this ReLU step
	double out_step7[64][1][1];

	// declare output for this fully connected step
	double out_step8[2][1][1];

	// declare output for this SoftMax step
	double out_step9[2][1][1];

};

#endif

