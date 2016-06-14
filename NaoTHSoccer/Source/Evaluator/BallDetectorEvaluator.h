#ifndef BALLDETECTOREVALUATOR_H
#define BALLDETECTOREVALUATOR_H

#include <string>

#include <Cognition/Modules/VisualCortex/BallDetector/BallDetector.h>

BEGIN_DECLARE_MODULE(BallDetectorEvaluator)

END_DECLARE_MODULE(BallDetectorEvaluator)

class BallDetectorEvaluator : public BallDetectorEvaluatorBase
{
public:
  BallDetectorEvaluator(const std::string& dir);
  virtual ~BallDetectorEvaluator();

  virtual void execute();
private:
  const std::string dir;
  BallDetector ballDetector;
};

#endif // BALLDETECTOREVALUATOR_H
