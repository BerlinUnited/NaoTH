#ifndef BALLDETECTOREVALUATOR_H
#define BALLDETECTOREVALUATOR_H

#include <string>

#include <Tools/Logfile/LogFileScanner.h>
#include <ModuleFramework/ModuleManager.h>
#include <Cognition/Modules/VisualCortex/BallDetector/BallDetector.h>

#include <map>

#include <picojson.h>

BEGIN_DECLARE_MODULE(BallDetectorEvaluator)
  PROVIDE(BallCandidates)
  PROVIDE(BallCandidatesTop)
END_DECLARE_MODULE(BallDetectorEvaluator)

class BallDetectorEvaluator : public ModuleManager, public BallDetectorEvaluatorBase
{
public:
  BallDetectorEvaluator(const std::string& file);
  virtual ~BallDetectorEvaluator();

  virtual void execute();
private:
  const std::string file;
  ModuleCreator<BallDetector>* ballDetector;
  LogFileScanner logFileScanner;
  LogFileScanner::FrameIterator currentFrame;

  std::map<int, bool> groundTruth;
};

#endif // BALLDETECTOREVALUATOR_H
