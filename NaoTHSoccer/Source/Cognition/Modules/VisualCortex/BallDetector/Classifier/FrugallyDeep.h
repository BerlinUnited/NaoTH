#ifndef _FRUGALLY_DEEP_H
#define _FRUGALLY_DEEP_H

#include "AbstractCNNClassifier.h"

// seems to be required on systems where clang uses gcc 13, otherwise following error might occur:
// LinuxToolchain/toolchain_native/extern/include/fplus/numeric.hpp:320:25: error: no type named 'uint64_t'
// in namespace 'std'; did you mean simply 'uint64_t'?
#include <cstdint>

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
 
    std::vector<fdeep::tensor> result;

};

#endif
