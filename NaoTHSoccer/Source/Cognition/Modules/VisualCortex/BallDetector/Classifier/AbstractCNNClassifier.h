#ifndef ABSTRACTCNNCLASSIFIER_H
#define ABSTRACTCNNCLASSIFIER_H

#include "Representations/Perception/BallCandidates.h"

class AbstractCNNClassifier
{
public:
    AbstractCNNClassifier(){}

    virtual ~AbstractCNNClassifier(){}

    virtual bool classify(const BallCandidates::Patch& p) = 0;

    // HACK! will be required and will be overloaded by "non cnn classifier" cvhaar classifier only
    virtual int classify(const BallCandidates::Patch &p, unsigned int /*minNeighbours*/, unsigned int /*windowSize*/) {
        return classify(p);
    }

    virtual float getBallConfidence() = 0;
    virtual float getNoballConfidence() = 0;
};

#endif // ABSTRACTCNNCLASSIFIER_H
