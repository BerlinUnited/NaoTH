#ifndef _CNN_synth_full_conv_H_
#define _CNN_synth_full_conv_H_

#include <limits>

#include <fstream>

#include "AbstractCNNClassifier.h"

class CNN_synth_full_conv : public AbstractCNNClassifier {

public:
	CNN_synth_full_conv(){out.open("example.txt");}

	~CNN_synth_full_conv(){out.close();}

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

	// declare output for this convolution step
	float out_step8[1][1][2];

	// declare output for this ReLU step
	float out_step9[1][1][2];

	// declare output for this SoftMax step
	float out_step10[1][1][2];

};

#endif

