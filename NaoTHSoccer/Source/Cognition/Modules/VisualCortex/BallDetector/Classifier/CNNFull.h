#ifndef _CNNFull_H_
#define _CNNFull_H_

#include <limits>

#include <fstream>

#include "AbstractCNNClassifier.h"

class CNNFull : public AbstractCNNClassifier {

public:
	CNNFull(){out.open("example.txt");}

	~CNNFull(){out.close();}

std::ofstream out;
	bool classify(const BallCandidates::Patch& p);

	// declare copied intial input array
	float out_step0[16][16][1];

	// declare output for this image input step
	float out_step1[16][16][1];

	// declare output for this convolution step
	float out_step2[8][8][4];

	// declare output for this ReLU step
	float out_step3[8][8][4];

	// declare output for this max polling step
	float out_step4[4][4][4];

	// declare output for this convolution step
	float out_step5[4][4][10];

	// declare output for this ReLU step
	float out_step6[4][4][10];

	// declare output for this max polling step
	float out_step7[3][3][10];

	// declare output for this fully connected step
	float out_step8[32][1][1];

	// declare output for this ReLU step
	float out_step9[32][1][1];

	// declare output for this fully connected step
	float out_step10[2][1][1];

	// declare output for this SoftMax step
	float out_step11[2][1][1];

};

#endif

