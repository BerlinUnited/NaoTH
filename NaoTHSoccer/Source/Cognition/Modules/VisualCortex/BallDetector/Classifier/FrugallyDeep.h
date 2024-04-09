#ifndef _FRUGALLY_DEEP_H
#define _FRUGALLY_DEEP_H

#include "AbstractCNNClassifier.h"

// seems to be required on systems where clang uses gcc 13, otherwise following error might occur:
// LinuxToolchain/toolchain_native/extern/include/fplus/numeric.hpp:320:25: error: no type named 'uint64_t'
// in namespace 'std'; did you mean simply 'uint64_t'?
#include <cstdint>

// NOTE: assumes frugally v0.15.20-p0 is used
#include <fdeep/fdeep.hpp>


class FrugallyDeep : public AbstractCNNFinder 
{
public:
    FrugallyDeep(std::string file, bool has_radius = false, bool has_center = false, bool has_confidence = true) :
      name(file),
      has_radius(has_radius),
      has_center(has_center),
      has_confidence(has_confidence)
    {
      model = std::make_shared<fdeep::model>(fdeep::load_model("Config/" + file));
    }
    
    virtual ~FrugallyDeep() {}

    virtual void predict(const BallCandidates::PatchYUVClassified& p, double meanBrightness);

    virtual double getRadius() const;
    virtual Vector2d getCenter() const;
    virtual double getBallConfidence() const;

private:
    const std::string name;
    bool has_radius;
    bool has_center;
    bool has_confidence;

    std::shared_ptr<fdeep::model> model;
    std::vector<fdeep::tensor> result;
};

#endif
