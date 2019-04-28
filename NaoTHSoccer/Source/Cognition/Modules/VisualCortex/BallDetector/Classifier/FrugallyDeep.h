#ifndef _FRUGALLY_DEEP_H
#define _FRUGALLY_DEEP_H

# include <emmintrin.h>
# include <math.h>

#include "AbstractCNNClassifier.h"

#include <fdeep/fdeep.hpp>


class FrugallyDeep : public AbstractCNNFinder {

public:
    FrugallyDeep(std::string file);
    virtual ~FrugallyDeep();

	void find(const BallCandidates::PatchYUVClassified& p, double meanBrightness);

	double getRadius();
    Vector2d getCenter();

private:
	std::shared_ptr<fdeep::model> model;
    double meanBrightness;

    std::vector<fdeep::tensor5> result;

};
# endif
