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
  if(argc > 3)
  {
    std::string type(argv[1]);
    if(type == "ball")
    {
      BallDetectorEvaluator evaluator(argv[2], argv[3]);
      evaluator.execute();
      return 0;
    }
  }

  std::cerr << "usage: evaluator <type> <model> <logfile>" << std::endl;
  std::cerr << "only \"ball\" is supported as type currently" << std::endl;
  return -1;
}
