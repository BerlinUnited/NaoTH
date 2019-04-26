#ifndef _CNN_THOMAS_BALLS_H
#define _CNN_THOMAS_BALLS_H

# include <emmintrin.h>
# include <math.h>

#include "AbstractCNNClassifier.h"

class CNN_THOMAS_BALLS{

public:
	int cnn(float x0[16][16][1],  double output_tensor[3][1][1]);
};
# endif
