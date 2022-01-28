#ifndef _FRUGALLY_DEEP_H
#define _FRUGALLY_DEEP_H



# include <emmintrin.h>

#include "AbstractCNNClassifier.h"

#include <fdeep/fdeep.hpp>


class FrugallyDeep : public AbstractCNNFinder {

public:
    FrugallyDeep(std::string file);
    virtual ~FrugallyDeep();

	void predict(const BallCandidates::PatchYUVClassified& p, double meanBrightness);

	double getRadius();
    Vector2d getCenter();

private:
	std::shared_ptr<fdeep::model> model;
 
    std::vector<fdeep::tensor5> result;

};

#endif