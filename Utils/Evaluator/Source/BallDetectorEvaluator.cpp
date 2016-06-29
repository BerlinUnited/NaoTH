#include "BallDetectorEvaluator.h"

#include <glib.h>
#include <glib/gstdio.h>

#include <strstream>
#include <fstream>
#include <sstream>

#include <Representations/Perception/BallCandidates.h>
#include <Cognition/Modules/VisualCortex/BallDetector/Tools/PatchWork.h>

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

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &fileArg, const std::string &modelDir)
  : fileArg(fileArg), modelDir(modelDir)
{

}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}


void BallDetectorEvaluator::execute()
{
  results.clear();
  std::string fileArgBase(g_path_get_basename(fileArg.c_str()));
  std::string outFileName = fileArgBase + ".html";

  std::list<std::string> models;
  if(modelDir.empty())
  {
    models.push_back("");
  }
  else
  {
    models = findModelNames();
  }

  double bestRecall90 = 0.0;
  ExperimentParameters bestRecallParam90;

  double bestRecall95 = 0.0;
  ExperimentParameters bestRecallParam95;

  double bestRecall99 = 0.0;
  ExperimentParameters bestRecallParam99;

  for(std::string modelName : models)
  {
    if(!modelName.empty())
    {
      classifier.loadModel(modelName);
    }

    // do experiment for different parameters
    for(unsigned int minNeighbours=0; minNeighbours <= 5; minNeighbours++)
    {
      for(unsigned int windowSize=12; windowSize <= 20; windowSize += 2)
      {
        ExperimentParameters params;
        params.minNeighbours = minNeighbours;
        params.maxWindowSize = windowSize;
        params.modelName = modelName;

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
                  r.totalSize += executeSingleFile(completeFileName, params, r);
                }
              }


            }
            g_dir_close(dir);
          }
        }
        else
        {
          // only one file
          r.totalSize = executeSingleFile(fileArg, params, r);
        }

        r.precision = 1.0;
        if(r.truePositives + r.falsePositives > 0)
        {
          r.precision = (double) r.truePositives / ((double) (r.truePositives + r.falsePositives));
        }
        r.recall = 0.0;
        if(r.truePositives + r.falsePositives > 0)
        {
          r.recall = (double) r.truePositives / ((double) (r.truePositives + r.falseNegatives));
        }

        results[params] = r;



        std::cout << "=============" << std::endl;
        std::cout << toDesc(params) << std::endl;

        std::cout << "precision: " << r.precision << std::endl;
        std::cout << "recall: " << r.recall << std::endl;

        if(r.precision >= 0.90 && bestRecall90 < r.recall)
        {
          bestRecallParam90 = params;
          bestRecall90 = r.recall;
        }

        if(r.precision >= 0.95 && bestRecall95 < r.recall)
        {
          bestRecallParam95 = params;
          bestRecall95 = r.recall;
        }

        if(r.precision >= 0.99 && bestRecall99 < r.recall)
        {
          bestRecallParam99 = params;
          bestRecall99 = r.recall;
        }

        if(bestRecall90 > 0.0)
        {
          std::cout << "-------------" << std::endl;
          std::cout << "best recall for precision >= 0.90: " << bestRecall90 << std::endl;
          std::cout << toDesc(bestRecallParam90) << std::endl;
        }


        if(bestRecall95 > 0.0)
        {
          std::cout << "-------------" << std::endl;
          std::cout << "best recall for precision >= 0.95: " << bestRecall95 << std::endl;
          std::cout << toDesc(bestRecallParam95) << std::endl;
        }

        if(bestRecall99 > 0.0)
        {
          std::cout << "-------------" << std::endl;
          std::cout << "best recall for precision >= 0.99: " << bestRecall99 << std::endl;
          std::cout << toDesc(bestRecallParam99) << std::endl;
        }

        std::cout << "=============" << std::endl;


      }
    }
  }
  outputResults(outFileName);
  std::cout << "Written detailed report to " << outFileName << std::endl;
}

void BallDetectorEvaluator::outputResults(std::string outFileName)
{
  base64::Encoder base64Encoder(64);

  std::ofstream html;
  html.open(outFileName);

  html << "<html>" << std::endl;
  html << "<head>" << std::endl;
  html << "<style>" << std::endl;
  // CSS
  html << "img.patch {width: 36px; height: 36px;}" << std::endl;
  html << "table, th, td {border: 1px solid; border-collapse: collapse; }" << std::endl;
  html << "td.cLow  {color: #FF0000; font-weight: bold;}" << std::endl;
  html << "td.cMiddle {color: #FF8806; font-weight: bold;}" << std::endl;
  html << "td.cGood {color: #94C705; font-weight: bold;}" << std::endl;
  html << "td.cExcellent {color: #00FF00; font-weight: bold;}" << std::endl;


  html << "th.sort-header::-moz-selection { background:transparent; }" << std::endl;
  html << "th.sort-header::selection      { background:transparent; }" << std::endl;
  html << "th.sort-header { cursor:pointer; }" << std::endl;
  html << "table th.sort-header:after {content:''; float:right; margin-top:7px; border-width:0 4px 4px; border-style:solid; border-color:#404040 transparent;  visibility:hidden;}" << std::endl;
  html << "table th.sort-header:hover:after {visibility:visible;}" << std::endl;
  html << "table th.sort-up:after, table th.sort-down:after, table th.sort-down:hover:after { visibility:visible; opacity:0.4; }" << std::endl;
  html << "table th.sort-up:after { border-bottom:none; border-width:4px 4px 0;}" << std::endl;


  html << "</style>" << std::endl;

  html << "<script src=\"tablesort.js\"></script>" << std::endl;
  html << "<script src=\"tablesort.number.js\"></script>" << std::endl;

  html << "</head>" << std::endl;

  html << "<body onload=\"if(window.Tablesort) {new Tablesort(document.getElementById('overviewTable'));}\" >" << std::endl;

  html << "<h1><a id=\"overview\">Overview (" << fileArg << ")</a></h1>" << std::endl;

  html << "<table id=\"overviewTable\">" << std::endl;
  html << "<thead>" << std::endl;
  html << "<tr>" << std::endl;
  html << "<th>" << "modelName" << "</th>" << std::endl;
  html << "<th data-sort-method='number'>" << "minNeighbours" << "</th>" << std::endl;
  html << "<th data-sort-method='number'>" << "windowSize" << "</th>" << std::endl;
  html << "<th data-sort-method='number''>" << "precision" << "</th>" << std::endl;
  html << "<th data-sort-method='number'>" << "recall" << "</th>" << std::endl;
  html << "<th class='no-sort'></th>" << std::endl;
  html << "</tr>" << std::endl;
  html << "</thead>" << std::endl;
  for(std::map<ExperimentParameters, ExperimentResult>::const_iterator it=results.begin();
      it != results.end(); it++)
  {
    const ExperimentParameters& params = it->first;
    const ExperimentResult& r = it->second;
    html << "<tr>" << std::endl;
    html << "<td>" << params.modelName << "</td>" << std::endl;
    html << "<td>" << params.minNeighbours << "</td>" << std::endl;
    html << "<td>" << params.maxWindowSize << "</td>" << std::endl;
    html << "<td class=\"" << precisionClass(r.precision) << "\">" << r.precision << "</td>" << std::endl;
    html << "<td class=\"" << recallClass(r.recall) << "\">" << r.recall << "</td>" << std::endl;
     html << "<td><a href=\"#result_" <<  toID(params) <<  "\">details</a></td>" << std::endl;
    html << "</tr>" << std::endl;
  }
  html << "</table>" << std::endl;


  for(std::map<ExperimentParameters, ExperimentResult>::const_iterator it=results.begin();
      it != results.end(); it++)
  {
    const ExperimentParameters& params = it->first;
    const ExperimentResult& r = it->second;


    html << "<h1><a id=\"result_" << toID(params) << "\">" << toDesc(params) << "</a></h1>" << std::endl;

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

std::list<std::string> BallDetectorEvaluator::findModelNames()
{
  std::list<std::string> result;
  std::string dirlocation = modelDir;

  if(g_file_test(modelDir.c_str(), G_FILE_TEST_IS_DIR))
  {
    GDir* dir = g_dir_open(dirlocation.c_str(), 0, NULL);
    if (dir != NULL)
    {
      const gchar* name;
      while ((name = g_dir_read_name(dir)) != NULL)
      {
        if (g_str_has_suffix(name, ".xml"))
        {
          std::string completeFileName = dirlocation + name;
          if (g_file_test(completeFileName.c_str(), G_FILE_TEST_EXISTS)
              && g_file_test(completeFileName.c_str(), G_FILE_TEST_IS_REGULAR))
          {
            result.push_back(name);
          }
        }

      }
      g_dir_close(dir);
    }
    result.sort();
  }
  else
  {
   std::string baseName(g_path_get_basename(modelDir.c_str()));
   result.push_back(baseName);
  }
  return result;
}

unsigned int BallDetectorEvaluator::executeSingleFile(std::string file, const ExperimentParameters& params, ExperimentResult& r)
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


    BallCandidates::PatchesList bottomPatches;
    if(!getBallCandidates().patchesYUVClassified.empty())
    {
      bottomPatches = PatchWork::toPatchList(getBallCandidates().patchesYUVClassified);
    }
    else if(!getBallCandidates().patches.empty())
    {
      bottomPatches = getBallCandidates().patches;
    }

    BallCandidates::PatchesList topPatches;
    if(!getBallCandidatesTop().patchesYUVClassified.empty())
    {
      topPatches = PatchWork::toPatchList(getBallCandidatesTop().patchesYUVClassified);
    }
    else if(!getBallCandidatesTop().patches.empty())
    {
      topPatches = getBallCandidatesTop().patches;
    }

    // The python script will always read the bottom patches first, thus in order to have the correct index
    // the loops have to bee in the same order.
    for(const BallCandidates::Patch& p : bottomPatches)
    {
      evaluatePatch(p, patchIdx++, CameraInfo::Bottom, expectedBallIdx, file, params, r);
    }

    for(const BallCandidates::Patch& p : topPatches)
    {
      evaluatePatch(p, patchIdx++, CameraInfo::Top, expectedBallIdx, file, params, r);
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
                                          const ExperimentParameters &params,
                                          ExperimentResult& r)
{
  // resize patch if necessary
  cv::Mat img;
  if(p.data.size() == 12*12)
  {
    cv::Mat wrappedImg(12, 12, CV_8U, (void*) p.data.data());
    img = wrappedImg; // only copies the header, not the image itself;
  }
  else if(p.data.size() == 24*24)
  {
    cv::Mat wrappedImg(24, 24, CV_8U, (void*) p.data.data());
    cv::resize(wrappedImg, img, cv::Size(12,12));
  }
  else if(p.data.size() == 36*36)
  {
    cv::Mat wrappedImg(36, 36, CV_8U, (void*) p.data.data());
    cv::resize(wrappedImg, img, cv::Size(12,12));
  }

  BallCandidates::Patch resizedPatch;
  resizedPatch.data = std::vector<unsigned char>(img.begin<unsigned char>(), img.end<unsigned char>());

  bool expected = expectedBallIdx.find(patchIdx) != expectedBallIdx.end();
  bool actual = classifier.classify(resizedPatch, params.minNeighbours, params.maxWindowSize) > 0;

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
  cv::Mat wrappedImg;
  if(p.data.size() == 12*12)
  {
    wrappedImg = cv::Mat(12, 12, CV_8UC1, (void*) p.data.data());
  }
  else if(p.data.size() == 24*24)
  {
    wrappedImg = cv::Mat(24, 24, CV_8UC1, (void*) p.data.data());
  }
  else if(p.data.size() == 36*36)
  {
    wrappedImg = cv::Mat(36, 36, CV_8UC1, (void*) p.data.data());
  }
  cv::transpose(wrappedImg, wrappedImg);
  //cv::flip(wrappedImg,wrappedImg, 1);

  std::vector<uchar> buffer;

  std::vector<int> params;
  params.push_back(cv::ImwriteFlags::IMWRITE_PNG_COMPRESSION);
  params.push_back(9);
  cv::imencode(".png", wrappedImg, buffer, params);

  return std::string(buffer.begin(), buffer.end());
}
