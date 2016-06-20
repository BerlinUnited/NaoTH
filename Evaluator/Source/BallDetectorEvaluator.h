#ifndef BALLDETECTOREVALUATOR_H
#define BALLDETECTOREVALUATOR_H

#include <string>

#include <Tools/Logfile/LogFileScanner.h>
#include <ModuleFramework/ModuleManager.h>
#include <Cognition/Modules/VisualCortex/BallDetector/BallDetector.h>
#include <Cognition/Modules/VisualCortex/BallDetector/Tools/CVClassifier.h>

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
  void evaluatePatch(const BallCandidates::Patch& p, unsigned int patchIdx, CameraInfo::CameraID camID);

private:
  const std::string file;
  LogFileScanner logFileScanner;
  LogFileScanner::FrameIterator currentFrame;

  std::set<unsigned int> expectedBallIdx;

  // TODO: allow more classifiers (including the ones that have the more complex filter logic)
  CVClassifier classifier;


  unsigned int truePositives, falsePositives, falseNegatives;
};

#endif // BALLDETECTOREVALUATOR_H
