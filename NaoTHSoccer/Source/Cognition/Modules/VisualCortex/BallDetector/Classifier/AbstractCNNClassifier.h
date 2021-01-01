#ifndef ABSTRACTCNNCLASSIFIER_H
#define ABSTRACTCNNCLASSIFIER_H

#include "Representations/Perception/BallCandidates.h"

class AbstractCNNClassifier
{
public:
    AbstractCNNClassifier(){}

    virtual ~AbstractCNNClassifier(){}

    virtual bool classify(const BallCandidates::Patch& p) = 0;
    virtual bool classify(const BallCandidates::PatchYUVClassified& p) = 0;

    virtual float getBallConfidence() = 0;
    virtual float getNoballConfidence() = 0;
};


class AbstractCNNFinder
{
public:
    AbstractCNNFinder(){}

    virtual ~AbstractCNNFinder(){}

    virtual void predict(const BallCandidates::PatchYUVClassified& p, double meanBrightness) = 0;

    virtual double getRadius() = 0;
    virtual Vector2d getCenter() = 0;

    virtual double getBallConfidence() {return getRadius(); }
};

#endif // ABSTRACTCNNCLASSIFIER_H
