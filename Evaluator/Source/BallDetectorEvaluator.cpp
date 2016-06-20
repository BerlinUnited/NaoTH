#include "BallDetectorEvaluator.h"

#include <strstream>
#include <fstream>

#include <Representations/Perception/BallCandidates.h>
#include <Cognition/Modules/VisualCortex/BallDetector/Tools/CVClassifier.h>

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &file)
  : file(file), logFileScanner(file)
{
  typedef std::vector<picojson::value> array;
  //typedef std::map<std::string, picojson::value> object;


  size_t dotPos = file.find_last_of('.');

  std::string jsonFile = dotPos == std::string::npos ? file : file.substr(0, dotPos) + ".json";
  std::cout << "loading ground truth from '" << jsonFile << "'" << std::endl;
  std::ifstream groundTruthStream(jsonFile);

  picojson::value parsedJson;
  picojson::parse(parsedJson, groundTruthStream);

  groundTruthStream.close();

  array ballIdx;
  if(parsedJson.get("ball").is<array>())
  {
    ballIdx = parsedJson.get("ball").get<array>();
  }

  for(picojson::value idx : ballIdx)
  {
    if(idx.is<double>())
    {
      groundTruth[(int) idx.get<double>()] = true;
    }
  }
}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}

void BallDetectorEvaluator::execute()
{
  // TODO: allow more classifiers (including the ones that have the actual filter logic)
  CVClassifier classifier;

  unsigned int patchIdx = 0;
  // read in each frame
  for(LogFileScanner::FrameIterator it = logFileScanner.begin(); it != logFileScanner.end(); it++)
  {
    // reset all existing candidates
    getBallCandidates().reset();
    getBallCandidatesTop().reset();

    LogFileScanner::Frame frame;
    logFileScanner.readFrame(*it, frame);

    // deserialize all ball candidates (bottom and top camera)
    auto frameBallCandidate = frame.find("BallCandidate");
    auto frameBallCandidateTop = frame.find("BallCandidateTop");
    if(frameBallCandidate!= frame.end())
    {
      std::istrstream stream(frameBallCandidate->second.data.data(), frameBallCandidate->second.data.size());
      naoth::Serializer<BallCandidates>::deserialize(stream, getBallCandidates());
    }
    if(frameBallCandidateTop != frame.end())
    {
      std::istrstream stream(frameBallCandidateTop->second.data.data(), frameBallCandidateTop->second.data.size());
      naoth::Serializer<BallCandidatesTop>::deserialize(stream, getBallCandidatesTop());
    }

    for(const BallCandidates::Patch& p : getBallCandidates().patches)
    {
      bool classifiedAsBall = classifier.classify(p, CameraInfo::Bottom);

      std::cout << patchIdx << ":" << classifiedAsBall << std::endl;
      patchIdx++;
    }

    for(const BallCandidates::Patch& p : getBallCandidatesTop().patches)
    {
      bool classifiedAsBall = classifier.classify(p, CameraInfo::Top);
      std::cout << patchIdx << ":" << classifiedAsBall << std::endl;
      patchIdx++;
    }

  }

}
