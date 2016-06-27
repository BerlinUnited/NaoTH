#ifndef BALLDETECTOREVALUATOR_H
#define BALLDETECTOREVALUATOR_H

#include <string>

#include <Tools/Logfile/LogFileScanner.h>
#include <ModuleFramework/ModuleManager.h>
#include <Cognition/Modules/VisualCortex/BallDetector/BallDetector.h>
#include <Cognition/Modules/VisualCortex/BallDetector/Tools/CVHaarClassifier.h>

#include <map>

#include <picojson.h>

BEGIN_DECLARE_MODULE(BallDetectorEvaluator)
  PROVIDE(BallCandidates)
  PROVIDE(BallCandidatesTop)
END_DECLARE_MODULE(BallDetectorEvaluator)

#define NAOTH_STRUCT_COMPARE(a, b) {if(a < b) {return true;} else if(a > b) {return false;}}

class BallDetectorEvaluator : public ModuleManager, public BallDetectorEvaluatorBase
{
public:
  BallDetectorEvaluator(const std::string& fileArg, const std::string& modelDir);
  virtual ~BallDetectorEvaluator();

  virtual void execute();

public:
  struct ErrorEntry
  {
    BallCandidates::Patch patch;
    unsigned int idx;
    std::string fileName;
  };

  struct ExperimentParameters
  {
    unsigned int minNeighbours;
    unsigned int maxWindowSize;

    std::string modelName;
  };


  struct cmpExperimentParameters {
      bool operator()(const ExperimentParameters& a, const ExperimentParameters& b) const {

        NAOTH_STRUCT_COMPARE(a.modelName, b.modelName);
        NAOTH_STRUCT_COMPARE(a.minNeighbours, b.minNeighbours);
        NAOTH_STRUCT_COMPARE(a.maxWindowSize, b.maxWindowSize);
        return false;
      }
  };


  struct ExperimentResult
  {
    unsigned int truePositives, falsePositives, falseNegatives, totalSize;


    std::list<ErrorEntry> falsePositivePatches;
    std::list<ErrorEntry> falseNegativePatches;

    double precision, recall;
  };

private:

  unsigned int executeSingleFile(std::string file, const ExperimentParameters &params, ExperimentResult &r);
  void evaluatePatch(const BallCandidates::Patch& p, unsigned int patchIdx, CameraInfo::CameraID camID,
                     const std::set<unsigned int> &expectedBallIdx, std::string fileName, const ExperimentParameters &params, ExperimentResult &r);

  int loadGroundTruth(std::string file, std::set<unsigned int> &expectedBallIdx);

  void outputResults(std::string outFileName);

  std::list<std::string> findModelNames();

  /**
   * @brief creates a Portable Graymap image output from a patch
   * @param p
   * @return
   */
  std::string createPGM(const BallCandidates::Patch& p);

  std::string createPNG(const BallCandidates::Patch& p);

  std::string toID(const ExperimentParameters& params)
  {
    return params.modelName + "_" + std::to_string(params.minNeighbours) + "_" + std::to_string(params.maxWindowSize);
  }

  std::string toDesc(const ExperimentParameters& params)
  {
    return "modelName=" + params.modelName + " minNeighbours=" + std::to_string(params.minNeighbours) + " maxWindowSize=" + std::to_string(params.maxWindowSize);
  }

private:
  const std::string fileArg;
  const std::string modelDir;

  // TODO: allow more classifiers (including the ones that have the more complex filter logic)
  CVHaarClassifier classifier;



  std::map<ExperimentParameters, ExperimentResult, cmpExperimentParameters> results;


};

#endif // BALLDETECTOREVALUATOR_H
