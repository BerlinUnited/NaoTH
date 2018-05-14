#include "BallDetectorEvaluator.h"

#include <glib.h>
#include <glib/gstdio.h>

#include <Tools/Extern/dirent.h>

#include <strstream>
#include <fstream>
#include <sstream>
#include <string>

#include <CTML/CTML.h>

#include <Representations/Perception/BallCandidates.h>
#include <Cognition/Modules/VisualCortex/BallDetector/Tools/PatchWork.h>

#include <Extern/libb64/encode.h>

#include <Tools/naoth_opencv.h>

#include <Cognition/Modules/VisualCortex/BallDetector/BallCandidateDetector.h>

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

BallDetectorEvaluator::BallDetectorEvaluator(const std::string &fileArg, const std::string &modelDir, int patchSize)
  : fileArg(fileArg), modelDir(modelDir), patchSize(patchSize)
{

}

BallDetectorEvaluator::~BallDetectorEvaluator()
{

}

BallDetectorEvaluator::ExperimentResult BallDetectorEvaluator::executeParam(
    const ExperimentParameters& params, const std::multimap<std::string, InputPatch>& imageSet)
{
  ExperimentResult r;

  r.truePositives = 0;
  r.falseNegatives = 0;
  r.falsePositives = 0;
  r.falsePositivePatches.clear();
  r.falseNegativePatches.clear();

  r.totalSize = executeSingleImageSet(imageSet, params, r);

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

  return r;
}

void BallDetectorEvaluator::executeCNNBall()
{
  std::cout << "Loading test image set from " << fileArg << std::endl;
  std::multimap<std::string, InputPatch> imagesByClasses = loadImageSets(fileArg);
  std::cout << "Loaded " << imagesByClasses.size() << " images." << std::endl;

  results.clear();
  std::string fileArgBase(g_path_get_basename(fileArg.c_str()));
  std::string outFileName = "cnn_ball_" + fileArgBase + ".html";

  bestRecall90 = 0.0;
  bestRecall95 = 0.0;
  bestRecall99 = 0.0;

  cnnClassifiers = BallCandidateDetector::createCNNMap();

  ExperimentParameters cnnParams;
  cnnParams.type = ExperimentParameters::Type::cnn;

  bestRecallParam90 = cnnParams;
  bestRecallParam95 = cnnParams;
  bestRecallParam99 = cnnParams;

  for(auto& classifierEntry : cnnClassifiers)
  {
    cnnParams.modelName = classifierEntry.first;
		results[cnnParams] = executeParam(cnnParams, imagesByClasses);
  }

  outputResults(outFileName);
  std::cout << "Written detailed report to " << outFileName << std::endl;
}

cv::Mat BallDetectorEvaluator::loadImage(std::string fullFilePath)
{
  //we assume input images are grayscale
  cv::Mat img = cv::imread(fullFilePath, CV_LOAD_IMAGE_GRAYSCALE);

  if (img.type() != CV_8UC1)
  {
    img.convertTo(img, CV_8UC1);
  }
  if (img.rows != patchSize || img.cols != patchSize)
  {
    cv::resize(img, img, cv::Size(patchSize, patchSize), cv::INTER_LINEAR);
  }
  return img;
}

std::multimap<std::string, BallDetectorEvaluator::InputPatch> BallDetectorEvaluator::loadImageSets(const std::string &rootDir, const std::string& pathSep)
{

  std::multimap<std::string, InputPatch> result;

  for(const std::string& className : findSubdirectories(rootDir))
  {
    for(const std::string& file : findDirectoryChildren(rootDir + pathSep + className))
    {
      std::string fullFilePath = rootDir + pathSep + className + pathSep + file;
      if(!isDirectory(fullFilePath) && hasFileSuffix(file, ".png"))
      {
        // get the image using opencv
        try
        {
          cv::Mat img = loadImage(fullFilePath);
          result.insert({className, {img, fullFilePath}});
        }
        catch(...)
        {
          // just ignore any invalid files
        }
      }
    }
  }
  return std::move(result);
}



void BallDetectorEvaluator::outputResults(std::string outFileName)
{
  base64::Encoder base64Encoder(64);

  CTML::Document doc;
  std::ofstream html;
  html.open(outFileName);

  doc.AddNodeToHead(CTML::Node("meta name = \"format-detection\" content = \"telephone=no\""));

  doc.AddNodeToHead(CTML::Node("style",
    "img.patch {width: 36px; height: 36px;}\n"
    "table, th, td {border: 1px solid; border-collapse: collapse; }\n"
    "td.cLow  {color: #FF0000; font-weight: bold;}\n"
    "td.cMiddle {color: #FF8806; font-weight: bold;}\n"
    "td.cGood {color: #94C705; font-weight: bold;}\n"
    "td.cExcellent {color: #00FF00; font-weight: bold;}\n"
    "th.sort-header::-moz-selection { background:transparent; }\n"
    "th.sort-header::selection      { background:transparent; }\n"
    "th.sort-header { cursor:pointer; }\n"
    "table th.sort-header:after {content:''; float:right; margin-top:7px; border-width:0 4px 4px; border-style:solid; border-color:#404040 transparent;  visibility:hidden;}\n"
    "table th.sort-header:hover:after {visibility:visible;}\n"
    "table th.sort-up:after, table th.sort-down:after, table th.sort-down:hover:after { visibility:visible; opacity:0.4; }\n"
    "table th.sort-up:after { border-bottom:none; border-width:4px 4px 0;}"));
  doc.AddNodeToHead(CTML::Node("script").SetAttribute("src", "tablesort.js"));
  doc.AddNodeToHead(CTML::Node("script").SetAttribute("src", "tablesort.number.js"));
  doc.AddNodeToHead(CTML::Node("script", "window.onload = function () {if(window.Tablesort) {new Tablesort(document.getElementById('overviewTable'));}}"));

  doc.AddNodeToBody(CTML::Node("h1").AppendChild(CTML::Node("a", "Overview (" + fileArg + ")").SetAttribute("id", "overview")));

  CTML::Node overviewTable("table");
  overviewTable.SetAttribute("id", "overviewTable");

  CTML::Node thead("thead");
  CTML::Node headRow("tr");
  headRow.AppendChild(CTML::Node("th", "modelName"));
  headRow.AppendChild(CTML::Node("th", "precision").SetAttribute("data-sort-method", "number"));
  headRow.AppendChild(CTML::Node("th", "recall").SetAttribute("data-sort-method", "number"));
  headRow.AppendChild(CTML::Node("th", "").SetAttribute("class", "no-sort"));

  thead.AppendChild(headRow);
  overviewTable.AppendChild(thead);

  for(std::map<ExperimentParameters, ExperimentResult>::const_iterator it=results.begin();
      it != results.end(); it++)
  {
    const ExperimentParameters& params = it->first;
    const ExperimentResult& r = it->second;

    CTML::Node tr("tr");
    tr.AppendChild(CTML::Node("td", params.modelName));
    tr.AppendChild(CTML::Node("td", std::to_string(r.precision)).SetAttribute("class", precisionClass(r.precision)));
    tr.AppendChild(CTML::Node("td", std::to_string(r.recall)).SetAttribute("class", recallClass(r.recall)));
    tr.AppendChild(CTML::Node("td").AppendChild(CTML::Node("a", "details").SetAttribute("href", "#result_" + toID(params))));

    overviewTable.AppendChild(tr);
  }


  doc.AddNodeToBody(overviewTable);

  for(std::map<ExperimentParameters, ExperimentResult>::const_iterator it=results.begin();
      it != results.end(); it++)
  {
    const ExperimentParameters& params = it->first;
    const ExperimentResult& r = it->second;

    doc.AddNodeToBody(CTML::Node("h1").AppendChild(CTML::Node("a", toDesc(params)).SetAttribute("id", "result_" + toID(params))));
    doc.AddNodeToBody(CTML::Node("h2", "Summary"));
    doc.AddNodeToBody(CTML::Node("p")
                      .AppendChild(CTML::Node("strong", "precision: " + std::to_string(r.precision)))
                      .AppendChild(CTML::Node("br"))
                      .AppendChild(CTML::Node("strong", "recall: " + std::to_string(r.recall)))
    );


    unsigned int numOfBalls = r.truePositives + r.falseNegatives;

    doc.AddNodeToBody(CTML::Node("p", "total number of samples: " + std::to_string(r.totalSize)
                                 + " (" + std::to_string(numOfBalls) + " balls, "
                                 + std::to_string(r.totalSize - numOfBalls) + " non-balls)"));

    doc.AddNodeToBody(CTML::Node("p").AppendChild(CTML::Node("a", "back to top").SetAttribute("href", "#overview")));

    doc.AddNodeToBody(CTML::Node("h2", "False Positives"));
    doc.AddNodeToBody(CTML::Node("p", "number: " + std::to_string(r.falsePositives)));

    CTML::Node divFP("div");

    for(std::list<ErrorEntry>::const_iterator it=r.falsePositivePatches.begin(); it != r.falsePositivePatches.end(); it++)
    {
      cv::Mat img = loadImage(it->fileName);
      // use a data URI to embed the image in PNG format
      std::string imgPNG = createPNG(img);
      divFP.AppendChild(CTML::Node("img")
                        .SetAttribute("title", it->fileName)
                        .SetAttribute("src", "data:image/png;base64," + base64Encoder.encode(imgPNG.c_str(), (int) imgPNG.size()))
      );
    }
    doc.AddNodeToBody(divFP);

    doc.AddNodeToBody(CTML::Node("h2", "False Negatives"));
    doc.AddNodeToBody(CTML::Node("p", "number: " + std::to_string(r.falseNegatives)));

    CTML::Node divFN("div");

    for(std::list<ErrorEntry>::const_iterator it=r.falseNegativePatches.begin(); it != r.falseNegativePatches.end(); it++)
    {
      // use a data URI to embed the image in PNG format
      cv::Mat img = loadImage(it->fileName);
      std::string imgPNG = createPNG(img);
      divFN.AppendChild(CTML::Node("img")
                        .SetAttribute("title", it->fileName)
                        .SetAttribute("src", "data:image/png;base64," + base64Encoder.encode(imgPNG.c_str(), (int) imgPNG.size()))
      );
    }

    doc.AddNodeToBody(divFN);
  }

  html << doc.ToString(CTML::Readability::MULTILINE);

  html.close();
}

unsigned int BallDetectorEvaluator::executeSingleImageSet(const std::multimap<std::string, InputPatch> &imageSet,
                                                          const ExperimentParameters& params, ExperimentResult& r)
{
  unsigned int numberOfImages = 0;

  for(const std::pair<std::string, InputPatch>& imageEntry : imageSet)
  {
    // only evaluate balls and non-balls
    if(imageEntry.first == "ball" || imageEntry.first == "noball")
    {
      evaluateImage(imageEntry.second.img, imageEntry.first == "ball", imageEntry.second.fileName, params, r);
      numberOfImages++;
    }
  }

  return numberOfImages;
}

void BallDetectorEvaluator::evaluateImage(cv::Mat img,
                                          bool ballExpected, std::string fileName,
																					const ExperimentParameters &params, ExperimentResult &r)
{

  BallCandidates::Patch patch;
  cv::Mat transposedImg;
  cv::transpose(img, transposedImg);
  patch.data = std::vector<unsigned char>(transposedImg.begin<unsigned char>(), transposedImg.end<unsigned char>());

  bool actual = false;  

	auto classifier = cnnClassifiers.find(params.modelName);
	if(classifier != cnnClassifiers.end())
	{
		if(classifier->second)
		{
		// the parameters are for the haar6 baseline
		actual = classifier->second->classify(patch);
		/*
		if(actual)
		{
			// also check the threshold
			actual = classifier->second->getBallConfidence() >= params.threshold;
		}
		*/
		}
	}
  

  if(ballExpected == actual)
  {
    if(ballExpected)
    {
      r.truePositives++;
    }
  }
  else
  {
    ErrorEntry error;
	  // error.patch= img;
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


std::string BallDetectorEvaluator::createPGM(const BallCandidates::Patch &p)
{
  std::stringstream str;

  unsigned int patchSize = static_cast<unsigned int>(std::sqrt(p.data.size()));

  // header (we are gray scale, thus P2)
  str << "P2\n";
  // the width and the height of the image
  str << patchSize << "\n" << patchSize << "\n";
  // the maximum value we use
  str << "255\n";

  // output each pixel
  for(unsigned int y=0; y < patchSize; y++)
  {
    for(unsigned int x=0; x < patchSize; x++)
    {
      str << (int) p.data[x*patchSize + y];
      if(x < patchSize - 1)
      {
        str << " ";
      }
    }
    str << "\n";
  }
  return str.str();
}

std::string BallDetectorEvaluator::createPNG(cv::Mat img)
{
  std::vector<uchar> buffer;

  std::vector<int> params;
  params.push_back(cv::ImwriteFlags::IMWRITE_PNG_COMPRESSION);
  params.push_back(9);
  cv::imencode(".png", img, buffer, params);

  return std::string(buffer.begin(), buffer.end());
}

bool BallDetectorEvaluator::isDirectory(const std::string &path)
{
  DIR* dirTest;
  if((dirTest = opendir(path.c_str())) != nullptr)
  {
    closedir(dirTest);

    return true;
  }
  return false;
}

std::list<std::string> BallDetectorEvaluator::findSubdirectories(const std::string &rootDir, std::string pathSep)
{
  std::list<std::string> result;

  for(const std::string& entry : findDirectoryChildren(rootDir))
  {
    if(isDirectory(rootDir + pathSep + entry))
    {
      result.push_back(entry);
    }
  }
  return std::move(result);
}

std::list<std::string> BallDetectorEvaluator::findDirectoryChildren(const std::string &rootDir)
{
  std::list<std::string> result;
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (rootDir.c_str())) != nullptr)
  {
    while ((ent = readdir (dir)) != nullptr)
    {
      std::string fileName(ent->d_name);
      if(fileName != "." && fileName != "..")
      {
        result.push_back(fileName);
      }

    }
    closedir (dir);
  }
  return std::move(result);
}

bool BallDetectorEvaluator::hasFileSuffix(const std::string &fileName, const std::string &fileSuffix)
{
  return fileName.size() > fileSuffix.size() &&
      fileName.compare(fileName.size() - fileSuffix.size(), fileSuffix.size(), fileSuffix) == 0;

}
