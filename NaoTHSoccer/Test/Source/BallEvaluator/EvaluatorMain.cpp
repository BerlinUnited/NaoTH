#include <iostream>

#include "BallDetectorEvaluator.h"

int main(int argc, char** argv)
{
  if(argc > 1)
  {
	BallDetectorEvaluator evaluator(argv[1], "");
	evaluator.executeCNNBall();

	return 0;
  }
  // <logfile> must be a path to a folder containing a ball and a noball subfolder
  std::cerr << "usage: evaluator <logfile> [<model-dir>]" << std::endl;
  return -1;
}
