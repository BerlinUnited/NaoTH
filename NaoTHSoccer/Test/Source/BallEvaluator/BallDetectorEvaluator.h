#ifndef BALLDETECTOREVALUATOR_H
#define BALLDETECTOREVALUATOR_H

#include <string>

#include <Cognition/Modules/VisualCortex/BallDetector/Classifier/AbstractCNNClassifier.h>

#include <map>

#include <opencv2/opencv.hpp>

#include <Tools/Extern/picojson.h>

#include <memory>


#define NAOTH_STRUCT_COMPARE(a, b) {if(a < b) {return true;} else if(a > b) {return false;}}

class BallDetectorEvaluator
{
public:
  BallDetectorEvaluator(const std::string& fileArg, const std::string& modelDir, int patchSize=16);
  virtual ~BallDetectorEvaluator();

  void executeCNNBall();

public:
  struct ErrorEntry
  {
    std::string fileName;
  };

  struct ExperimentParameters
  {
		//TODO delete type
    enum class Type {cnn};

    Type type;

    double threshold;

    std::string modelName;
  };


  struct cmpExperimentParameters {
      bool operator()(const ExperimentParameters& a, const ExperimentParameters& b) const {

        NAOTH_STRUCT_COMPARE(a.type, b.type);
        NAOTH_STRUCT_COMPARE(a.modelName, b.modelName);
        NAOTH_STRUCT_COMPARE(a.threshold, b.threshold);
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

  struct InputPatch
  {
    cv::Mat img;
    std::string fileName;
  };

private:

  std::multimap<std::string, InputPatch> loadImageSets(const std::string& rootDir, const std::string &pathSep="/");  
  cv::Mat loadImage(std::string fullFilePath);
  
  ExperimentResult executeParam(const ExperimentParameters& params, const std::multimap<std::string, InputPatch> &imageSet);
  unsigned int executeSingleImageSet(const std::multimap<std::string, InputPatch> &imageSet, const ExperimentParameters &params, ExperimentResult &r);

  void evaluateImage(cv::Mat img, bool ballExpected, std::string fileName, const ExperimentParameters &params, ExperimentResult &r);

  void outputResults(std::string outFileName);

  std::list<std::string> findHaarModelNames();

  /**
   * @brief creates a Portable Graymap image output from a patch
   * @param p
   * @return
   */
  std::string createPGM(const BallCandidates::Patch& p);

  std::string createPNG(cv::Mat img);

  std::string toID(const ExperimentParameters& params)
  {
    if(params.type == ExperimentParameters::Type::cnn)
    {
      return params.modelName;
    }
    // FIXME: what shall I return?
    assert(false);
    return "";
  }

  std::string toDesc(const ExperimentParameters& params)
  {
    if(params.type == ExperimentParameters::Type::cnn)
    {
      return params.modelName + " (CNN)";
    }
    else
    {
      // fallback
      return params.modelName;
    }
  }

  std::string precisionClass(double precision)
  {
    if(precision < 0.9)
    {
      return "cLow";
    }
    else if(precision < 0.95)
    {
      return "cMiddle";
    }
    else if(precision < 0.99)
    {
      return "cGood";
    }
    else
    {
      return "cExcellent";
    }
  }

  std::string recallClass(double precision)
  {
    if(precision < 0.5)
    {
      return "cLow";
    }
    else if(precision < 0.75)
    {
      return "cMiddle";
    }
    else if(precision < 0.90)
    {
      return "cGood";
    }
    else
    {
      return "cExcellent";
    }
  }

  /* helper  functions for file system access */
  bool isDirectory(const std::string& path);
  std::list<std::string> findSubdirectories(const std::string &rootDir, std::string pathSep="/");
  std::list<std::string> findDirectoryChildren(const std::string& rootDir);
  bool hasFileSuffix(const std::string& fileName, const std::string& fileSuffix);

private:
  const std::string fileArg;
  const std::string modelDir;

  const int patchSize;

  // TODO: allow more classifiers (including the ones that have the more complex filter logic)
  std::map<std::string, std::shared_ptr<AbstractCNNClassifier>> cnnClassifiers;

  std::map<ExperimentParameters, ExperimentResult, cmpExperimentParameters> results;

  double bestRecall90;
  double bestRecall95;
  double bestRecall99;
  ExperimentParameters bestRecallParam90;
  ExperimentParameters bestRecallParam95;
  ExperimentParameters bestRecallParam99;



};

#endif // BALLDETECTOREVALUATOR_H
