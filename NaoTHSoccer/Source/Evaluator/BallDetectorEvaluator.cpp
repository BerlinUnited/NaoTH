#include "BallDetectorEvaluator.h"

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &dir)
  : dir(dir)
{

}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}

void BallDetectorEvaluator::execute()
{
  ballDetector.execute();
}
