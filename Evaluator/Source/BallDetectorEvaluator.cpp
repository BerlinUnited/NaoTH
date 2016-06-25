#include "BallDetectorEvaluator.h"

#include <glib.h>
#include <glib/gstdio.h>

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
  : fileArg(fileArg), minNeighbours(0)
{

}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}


void BallDetectorEvaluator::execute()
{
  std::string outFileName = "HaarBallDetector_Evaluation.html";


  std::ofstream html;
  html.open(outFileName);

  html << "<html>" << std::endl;
  html << "<head>" << std::endl;
  html << "<style>" << std::endl;
  // CSS
  html << "img.patch {width: 36px; height: 36px}" << std::endl;
  html << "</style>" << std::endl;
  html << "</head>" << std::endl;

  html << "<body>" << std::endl;

  html << "<h1><a id=\"overview\">Overview</a></h1>" << std::endl;


  // do experiment for different parameters
  for(minNeighbours=0; minNeighbours < 6; minNeighbours++)
  {

    ExperimentResult r;

    r.truePositives = 0;
    r.falseNegatives = 0;
    r.falsePositives = 0;
    r.falsePositivePatches.clear();
    r.falseNegativePatches.clear();
    r.totalSize = 0;


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
              r.totalSize += executeSingleFile(completeFileName, r);
            }
          }


        }
        g_dir_close(dir);
      }
    }
    else
    {
      // only one file
      r.totalSize = executeSingleFile(fileArg, r);
    }

    r.precision = 1.0;
    if(r.truePositives + r.falsePositives > 0)
    {
      r.precision = (double) r.truePositives / ((double) (r.truePositives + r.falsePositives));
    }
    r.recall = 1.0;
    if(r.truePositives + r.falsePositives > 0)
    {
      r.recall = (double) r.truePositives / ((double) (r.truePositives + r.falseNegatives));
    }


    std::cout << "=============" << std::endl;
    std::cout << "minNeighbours=" << minNeighbours << std::endl;

    std::cout << "precision: " << r.precision << std::endl;
    std::cout << "recall: " << r.recall << std::endl;

    std::cout << "=============" << std::endl;
    std::cout << "Written detailed report to " << outFileName << std::endl;

    base64::Encoder base64Encoder(64);


    html << "<h1>minNeighbours=" << minNeighbours << "</h1>" << std::endl;

    html << "<h2>Summary</h2>" << std::endl;
    html << "<p><strong>precision: " << r.precision << "<br />recall: " << r.recall << "</strong></p>" << std::endl;


    unsigned int numOfBalls = r.truePositives + r.falseNegatives;
    html << "<p>total number of samples: " << r.totalSize << " (" << numOfBalls << " balls, " << (r.totalSize - numOfBalls) << " non-balls)</p>" << std::endl;

    html << "<p><a href=\"#overview\">back to top</a></p>" << std::endl;

    html << "<h2>False Positives</h2>" << std::endl;

    html << "<p>number: " << r.falsePositives << "</p>" << std::endl;

    html << "<div>" << std::endl;
    for(std::list<ErrorEntry>::const_iterator it=r.falsePositivePatches.begin(); it != r.falsePositivePatches.end(); it++)
    {
      // use a data URI to embed the image in PNG format
      std::string imgPNG = createPNG(it->patch);
      html << "<img class=\"patch\" title=\"" << it->idx << "@" << it->fileName
           << "\" src=\"data:image/png;base64," << base64Encoder.encode(imgPNG.c_str(), (int) imgPNG.size())  << "\" />" << std::endl;
    }
    html << "</div>" << std::endl;

    html << "<h1>False Negatives</h1>" << std::endl;

    html << "<p>number: " << r.falseNegatives << "</p>" << std::endl;

    html << "<div>" << std::endl;
    for(std::list<ErrorEntry>::const_iterator it=r.falseNegativePatches.begin(); it != r.falseNegativePatches.end(); it++)
    {
      // use a data URI to embed the image in PNG format
      std::string imgPNG = createPNG(it->patch);
      html << "<img class=\"patch\" title=\"" << it->idx << "@" << it->fileName
           << "\" src=\"data:image/png;base64," << base64Encoder.encode(imgPNG.c_str(), (int) imgPNG.size())  << "\" />" << std::endl;
    }
    html << "</div>" << std::endl;

  }


  html << "</body>" << std::endl;
  html.close();
}

unsigned int BallDetectorEvaluator::executeSingleFile(std::string file, ExperimentResult& r)
{
  LogFileScanner logFileScanner(file);

  std::set<unsigned int> expectedBallIdx;
  unsigned int maxValidIdx = loadGroundTruth(file, expectedBallIdx);

  unsigned int patchIdx = 0;

  // read in each frame
  LogFileScanner::FrameIterator secondLastFrame = logFileScanner.end();
  secondLastFrame--;
  for(LogFileScanner::FrameIterator it = logFileScanner.begin(); it != secondLastFrame; it++)
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
      evaluatePatch(p, patchIdx++, CameraInfo::Bottom, expectedBallIdx, file, r);
    }

    for(const BallCandidates::Patch& p : getBallCandidatesTop().patches)
    {
      evaluatePatch(p, patchIdx++, CameraInfo::Bottom, expectedBallIdx, file, r);
    }

    if(patchIdx >= maxValidIdx)
    {
      break;
    }
  }

  return patchIdx;
}

void BallDetectorEvaluator::evaluatePatch(const BallCandidates::Patch &p, unsigned int patchIdx,
                                          CameraInfo::CameraID camID,
                                          const std::set<unsigned int>& expectedBallIdx,
                                          std::string fileName,
                                          ExperimentResult& r)
{
  bool expected = expectedBallIdx.find(patchIdx) != expectedBallIdx.end();
  bool actual = classifier.classify(p, minNeighbours) > 0;

  if(expected == actual)
  {
    if(expected)
    {
      r.truePositives++;
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
      r.falsePositivePatches.push_back(error);
      r.falsePositives++;
    }
    else
    {
      r.falseNegativePatches.push_back(error);
      r.falseNegatives++;
    }
  }
}

int BallDetectorEvaluator::loadGroundTruth(std::string file, std::set<unsigned int>& expectedBallIdx)
{
  typedef std::vector<picojson::value> array;
  typedef std::map<std::string, picojson::value> object;

  int maxValidIdx = 0;


  size_t dotPos = file.find_last_of('.');

  std::string jsonFile = dotPos == std::string::npos ? file : file.substr(0, dotPos) + ".json";
  std::cout << "loading ground truth from '" << jsonFile << "'" << std::endl;
  std::ifstream groundTruthStream(jsonFile);

  picojson::value parsedJson;
  picojson::parse(parsedJson, groundTruthStream);

  groundTruthStream.close();

  array ballIdx;
  array noBallIdx;
  if(parsedJson.is<object>())
  {
    if(parsedJson.get("ball").is<array>())
    {
      ballIdx = parsedJson.get("ball").get<array>();
    }

    if(parsedJson.get("noball").is<array>())
    {
      noBallIdx = parsedJson.get("noball").get<array>();
    }

    for(picojson::value idx : ballIdx)
    {
      if(idx.is<double>())
      {
        int idxVal = static_cast<int>(idx.get<double>());
        expectedBallIdx.insert(idxVal);
        maxValidIdx = std::max(maxValidIdx, idxVal);
      }
    }

    for(picojson::value idx : noBallIdx)
    {
      if(idx.is<double>())
      {
        int idxVal = static_cast<int>(idx.get<double>());
        maxValidIdx = std::max(maxValidIdx, idxVal);
      }
    }
  }


  return maxValidIdx;
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
