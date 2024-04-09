#ifndef ABSTRACTCNNCLASSIFIER_H
#define ABSTRACTCNNCLASSIFIER_H

#include "Representations/Perception/BallCandidates.h"

class AbstractCNNFinder
{
public:
    AbstractCNNFinder(){}
    virtual ~AbstractCNNFinder(){}

    virtual void predict(const BallCandidates::PatchYUVClassified& p, double meanBrightness) = 0;

    virtual double getRadius() const = 0;
    virtual Vector2d getCenter() const = 0;
    virtual double getBallConfidence() const = 0;
};

#endif // ABSTRACTCNNCLASSIFIER_H
