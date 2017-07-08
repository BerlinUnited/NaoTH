#ifndef ABSTRACTCCNCLASSIFIER_H
#define ABSTRACTCCNCLASSIFIER_H

#include "Representations/Perception/BallCandidates.h"

class AbstractCCNClassifier
{
public:
    AbstractCCNClassifier(){}

    virtual ~AbstractCCNClassifier(){}

    virtual bool classify(const BallCandidates::Patch& p) = 0;
};

#endif // ABSTRACTCCNCLASSIFIER_H
