#include "BallDetectorEvaluator.h"

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &dir)
  : dir(dir)
{
  ballDetector = registerModule<BallDetector>("BallDetector", true);
}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}

void BallDetectorEvaluator::execute()
{
  if(ballDetector != NULL)
  {
    ballDetector->execute();
  }
}
