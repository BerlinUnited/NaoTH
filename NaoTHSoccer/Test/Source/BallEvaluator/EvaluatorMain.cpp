#include <iostream>

#include "BallDetectorEvaluator.h"

/**
 * @brief main function for the evaluator
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv)
{
  if(argc > 2)
  {
    std::string type(argv[1]);
    if(type == "haar")
    {
      if(argc > 3)
      {
        BallDetectorEvaluator evaluator(argv[2], argv[3]);
        evaluator.executeHaarBall();
      }
      else
      {
        BallDetectorEvaluator evaluator(argv[2], "");
        evaluator.executeHaarBall();
      }
      return 0;
    }
    else if(type == "cnn")
    {
      if(argc > 2)
      {
        BallDetectorEvaluator evaluator(argv[2], "");
        evaluator.executeCNNBall();

        return 0;
      }
    }
  }

  std::cerr << "usage: evaluator <type> <logfile> [<model-dir>]" << std::endl;
  std::cerr << "only \"haar\" and \"cnn\" are supported as type currently" << std::endl;
  return -1;
}
