#include "BallDetectorEvaluator.h"

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &dir)
  : file(dir), logFileScanner(dir)
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
    for(LogFileScanner::FrameIterator it = logFileScanner.begin(); it != logFileScanner.end(); it++)
    {

      LogFileScanner::Frame frame;

      std::cout << "Testing frame " << *it << std::endl;
      logFileScanner.readFrame(*it, frame);

      ballDetector->execute();
    }
  }
}
