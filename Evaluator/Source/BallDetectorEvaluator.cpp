#include "BallDetectorEvaluator.h"

#include <strstream>
#include <fstream>
#include <sstream>

#include <Representations/Perception/BallCandidates.h>

#include <Extern/libb64/encode.h>

#include <Tools/naoth_opencv.h>

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 5) || (__GNUC__ > 4) // version >= 4.6
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 8) || (__GNUC__ > 4) // version >= 4.9
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif
#endif

#include <opencv2/imgcodecs/imgcodecs.hpp>

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 5) || (__GNUC__ > 4)  // version >= 4.6
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic error "-Wconversion"
#if (__GNUC__ > 3 && __GNUC_MINOR__ > 8) || (__GNUC__ > 4) // version >= 4.9
#pragma GCC diagnostic error "-Wfloat-conversion"
#endif
#endif

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
  falsePositivePatches.clear();
  falseNegativePatches.clear();

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

  base64::Encoder base64Encoder(64);

  //TODO: also output a simple HTML file with the actual images
  std::ofstream html;
  html.open("BallEvaluator_output.html");

  html << "<html>" << std::endl;
  html << "<head>" << std::endl;
  // TODO: write some CSS?
  html << "<style>" << std::endl;
  html << "img.patch {width: 36px; height: 36px}" << std::endl;
  html << "</style>" << std::endl;
  html << "</head>" << std::endl;

  html << "<body>" << std::endl;

  html << "<h1>Summary</h1>" << std::endl;
  html << "<p>precision: " << precision << "<br />recall: " << recall << "</p>" << std::endl;

  html << "<h1>False Positives</h1>" << std::endl;

  for(std::list<BallCandidates::Patch>::const_iterator it=falsePositivePatches.begin(); it != falsePositivePatches.end(); it++)
  {
    // use a data URI to embed the image in PNG format
    std::string imgPNG = createPNG(*it);
    html << "<img class=\"patch\" src=\"data:image/png;base64," << base64Encoder.encode(imgPNG.c_str(), (int) imgPNG.size())  << "\" />" << std::endl;
  }

  html << "<h1>False Negatives</h1>" << std::endl;
  for(std::list<BallCandidates::Patch>::const_iterator it=falseNegativePatches.begin(); it != falseNegativePatches.end(); it++)
  {
    // use a data URI to embed the image in PNG format
    std::string imgPNG = createPNG(*it);
    html << "<img class=\"patch\" src=\"data:image/png;base64," << base64Encoder.encode(imgPNG.c_str(), (int) imgPNG.size())  << "\" />" << std::endl;
  }
  html << "</body>" << std::endl;
  html.close();

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
      falsePositivePatches.push_back(p);
      falsePositives++;
    }
    else
    {
      falseNegativePatches.push_back(p);
      falseNegatives++;
    }
  }
}

std::string BallDetectorEvaluator::createPGM(const BallCandidates::Patch &p)
{
  std::stringstream str;

  // header (we are gray scale, thus P2)
  str << "P2\n";
  // the width and the height of the image
  str << BallCandidates::Patch::SIZE << "\n" << BallCandidates::Patch::SIZE << "\n";
  // the maximum value we use
  str << "255\n";

  // output each pixel
  for(unsigned int y=0; y < BallCandidates::Patch::SIZE; y++)
  {
    for(unsigned int x=0; x < BallCandidates::Patch::SIZE; x++)
    {
      str << (int) p.data[x*BallCandidates::Patch::SIZE + y];
      if(x < BallCandidates::Patch::SIZE - 1)
      {
        str << " ";
      }
    }
    str << "\n";
  }
  return str.str();
}

std::string BallDetectorEvaluator::createPNG(const BallCandidates::Patch &p)
{
  cv::Mat wrappedImg(12, 12, CV_8UC1, (void*) p.data.data());

  std::vector<uchar> buffer;

  cv::imencode(".png", wrappedImg, buffer);

  return std::string(buffer.begin(), buffer.end());
}
