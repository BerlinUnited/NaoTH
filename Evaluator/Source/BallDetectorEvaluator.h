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
  BallDetectorEvaluator(const std::string& fileArg);
  virtual ~BallDetectorEvaluator();

  virtual void execute();

private:
  unsigned int executeSingleFile(std::string file);
  void evaluatePatch(const BallCandidates::Patch& p, unsigned int patchIdx, CameraInfo::CameraID camID, const std::set<unsigned int> &expectedBallIdx);

  std::set<unsigned int> loadGroundTruth(std::string file);

  /**
   * @brief creates a Portable Graymap image output from a patch
   * @param p
   * @return
   */
  std::string createPGM(const BallCandidates::Patch& p);

  std::string createPNG(const BallCandidates::Patch& p);

private:
  const std::string fileArg;


  // TODO: allow more classifiers (including the ones that have the more complex filter logic)
  CVClassifier classifier;


  unsigned int truePositives, falsePositives, falseNegatives;

  std::list<std::pair<BallCandidates::Patch, unsigned int>> falsePositivePatches;
  std::list<std::pair<BallCandidates::Patch, unsigned int>> falseNegativePatches;
};

#endif // BALLDETECTOREVALUATOR_H
