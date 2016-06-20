#include "BallDetectorEvaluator.h"

#include <strstream>
#include <fstream>

#include <Representations/Perception/BallCandidates.h>

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &file)
  : file(file), logFileScanner(file),
    truePositives(0), falsePositives(0), falseNegatives(0)
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
      expectedBallIdx.insert((unsigned int) idx.get<double>());
    }
  }
}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}

void BallDetectorEvaluator::execute()
{
  unsigned int patchIdx = 0;
  truePositives = falseNegatives = falsePositives = 0;

  // read in each frame
  for(LogFileScanner::FrameIterator it = logFileScanner.begin(); it != logFileScanner.end(); it++)
  {
    // reset all existing candidates
    getBallCandidates().reset();
    getBallCandidatesTop().reset();

    LogFileScanner::Frame frame;
    logFileScanner.readFrame(*it, frame);

    // deserialize all ball candidates (bottom and top camera)
    auto frameBallCandidate = frame.find("BallCandidates");
    auto frameBallCandidateTop = frame.find("BallCandidatesTop");
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

    // The python script will always read the bottom patches first, thus in order to have the correct index
    // the loops have to bee in the same order.

    for(const BallCandidates::Patch& p : getBallCandidates().patches)
    {
      evaluatePatch(p, patchIdx++, CameraInfo::Bottom);
    }

    for(const BallCandidates::Patch& p : getBallCandidatesTop().patches)
    {
      evaluatePatch(p, patchIdx++, CameraInfo::Bottom);
    }
  }


  std::cout << "true positives: " << truePositives << " false positives: " << falsePositives << " false negatives: " << falseNegatives << std::endl;

  double precision = (double) truePositives / ((double) (truePositives + falsePositives));
  double recall = (double) truePositives / ((double) (truePositives + falseNegatives));

  std::cout << "=============" << std::endl;

  std::cout << "precision: " << precision << std::endl;
  std::cout << "recall: " << recall << std::endl;

}

void BallDetectorEvaluator::evaluatePatch(const BallCandidates::Patch &p, unsigned int patchIdx, CameraInfo::CameraID camID)
{
  bool expected = expectedBallIdx.find(patchIdx) != expectedBallIdx.end();
  bool actual = classifier.classify(p, camID);

  if(expected == actual)
  {
    if(expected)
    {
      truePositives++;
    }
  }
  else
  {
    if(actual)
    {
      falsePositives++;
    }
    else
    {
      falseNegatives++;
    }
  }
}
