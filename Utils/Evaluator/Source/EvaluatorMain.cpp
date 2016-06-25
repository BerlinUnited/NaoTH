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
    if(type == "ball_haar")
    {
      BallDetectorEvaluator evaluator(argv[2]);
      evaluator.execute();
      return 0;
    }
  }

  std::cerr << "usage: evaluator <type> <logfile>" << std::endl;
  std::cerr << "only \"ball_haar\" is supported as type currently" << std::endl;
  return -1;
}
