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

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &fileArg)
  : fileArg(fileArg),
    truePositives(0), falsePositives(0), falseNegatives(0)
{

}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}

void BallDetectorEvaluator::execute()
{

  truePositives = falseNegatives = falsePositives = 0;
  falsePositivePatches.clear();
  falseNegativePatches.clear();


  unsigned int totalSize = 0;


  if(g_file_test(fileArg.c_str(), G_FILE_TEST_IS_DIR))
  {
    std::string dirlocation = fileArg;
    if (!g_str_has_suffix(dirlocation.c_str(), "/"))
    {
      dirlocation = dirlocation + "/";
    }

    GDir* dir = g_dir_open(dirlocation.c_str(), 0, NULL);
    if (dir != NULL)
    {
      const gchar* name;
      while ((name = g_dir_read_name(dir)) != NULL)
      {
        if (g_str_has_suffix(name, ".log"))
        {
          std::string completeFileName = dirlocation + name;
          if (g_file_test(completeFileName.c_str(), G_FILE_TEST_EXISTS)
              && g_file_test(completeFileName.c_str(), G_FILE_TEST_IS_REGULAR))
          {
            totalSize += executeSingleFile(completeFileName);
          }
        }


      }
      g_dir_close(dir);
    }
  }
  else
  {
    // only one file
    totalSize = executeSingleFile(fileArg);
  }

  double precision = 1.0;
  if(truePositives + falsePositives > 0)
  {
    precision = (double) truePositives / ((double) (truePositives + falsePositives));
  }
  double recall = 1.0;
  if(truePositives + falsePositives > 0)
  {
    recall = (double) truePositives / ((double) (truePositives + falseNegatives));
  }

  std::cout << "=============" << std::endl;

  std::cout << "precision: " << precision << std::endl;
  std::cout << "recall: " << recall << std::endl;

  std::cout << "=============" << std::endl;
  std::cout << "Written detailed report to BallEvaluator_output.html" << std::endl;

  base64::Encoder base64Encoder(64);

  std::ofstream html;
  html.open("BallEvaluator_output.html");

  html << "<html>" << std::endl;
  html << "<head>" << std::endl;
  html << "<style>" << std::endl;
  // CSS
  html << "img.patch {width: 36px; height: 36px}" << std::endl;
  html << "</style>" << std::endl;
  html << "</head>" << std::endl;

  html << "<body>" << std::endl;

  html << "<h1>Summary</h1>" << std::endl;
  html << "<p><strong>precision: " << precision << "<br />recall: " << recall << "</strong></p>" << std::endl;


  unsigned int numOfBalls = truePositives + falseNegatives;
  html << "<p>total number of samples: " << totalSize << " (" << numOfBalls << " balls, " << (totalSize - numOfBalls) << " non-balls)</p>" << std::endl;

  html << "<h1>False Positives</h1>" << std::endl;

  html << "<p>number: " << falsePositives << "</p>" << std::endl;

  html << "<div>" << std::endl;
  for(std::list<ErrorEntry>::const_iterator it=falsePositivePatches.begin(); it != falsePositivePatches.end(); it++)
  {
    // use a data URI to embed the image in PNG format
    std::string imgPNG = createPNG(it->patch);
    html << "<img class=\"patch\" title=\"" << it->idx << "@" << it->fileName
         << "\" src=\"data:image/png;base64," << base64Encoder.encode(imgPNG.c_str(), (int) imgPNG.size())  << "\" />" << std::endl;
  }
  html << "</div>" << std::endl;

  html << "<h1>False Negatives</h1>" << std::endl;

  html << "<p>number: " << falseNegatives << "</p>" << std::endl;

  html << "<div>" << std::endl;
  for(std::list<ErrorEntry>::const_iterator it=falseNegativePatches.begin(); it != falseNegativePatches.end(); it++)
  {
    // use a data URI to embed the image in PNG format
    std::string imgPNG = createPNG(it->patch);
    html << "<img class=\"patch\" title=\"" << it->idx << "@" << it->fileName
         << "\" src=\"data:image/png;base64," << base64Encoder.encode(imgPNG.c_str(), (int) imgPNG.size())  << "\" />" << std::endl;
  }
  html << "</div>" << std::endl;
  html << "</body>" << std::endl;
  html.close();

}

unsigned int BallDetectorEvaluator::executeSingleFile(std::string file)
{
  LogFileScanner logFileScanner(file);

  std::set<unsigned int> expectedBallIdx = loadGroundTruth(file);

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
    LogFileScanner::Frame::const_iterator frameBallCandidate = frame.find("BallCandidates");
    LogFileScanner::Frame::const_iterator frameBallCandidateTop = frame.find("BallCandidatesTop");
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
      evaluatePatch(p, patchIdx++, CameraInfo::Bottom, expectedBallIdx, file);
    }

    for(const BallCandidates::Patch& p : getBallCandidatesTop().patches)
    {
      evaluatePatch(p, patchIdx++, CameraInfo::Bottom, expectedBallIdx, file);
    }
  }

  return patchIdx;
}

void BallDetectorEvaluator::evaluatePatch(const BallCandidates::Patch &p, unsigned int patchIdx, CameraInfo::CameraID camID,
                                          const std::set<unsigned int>& expectedBallIdx,
                                          std::string fileName)
{
  bool expected = expectedBallIdx.find(patchIdx) != expectedBallIdx.end();
  bool actual = classifier.classify(p) > 0;

  if(expected == actual)
  {
    if(expected)
    {
      truePositives++;
    }
  }
  else
  {
    ErrorEntry error;
    error.patch= p;
    error.idx = patchIdx;
    error.fileName = fileName;
    if(actual)
    {
      falsePositivePatches.push_back(error);
      falsePositives++;
    }
    else
    {
      falseNegativePatches.push_back(error);
      falseNegatives++;
    }
  }
}

std::set<unsigned int> BallDetectorEvaluator::loadGroundTruth(std::__cxx11::string file)
{
  typedef std::vector<picojson::value> array;
  typedef std::map<std::string, picojson::value> object;

  std::set<unsigned int> expectedBallIdx;

  size_t dotPos = file.find_last_of('.');

  std::string jsonFile = dotPos == std::string::npos ? file : file.substr(0, dotPos) + ".json";
  std::cout << "loading ground truth from '" << jsonFile << "'" << std::endl;
  std::ifstream groundTruthStream(jsonFile);

  picojson::value parsedJson;
  picojson::parse(parsedJson, groundTruthStream);

  groundTruthStream.close();

  array ballIdx;
  if(parsedJson.is<object>())
  {
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

  return expectedBallIdx;
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
