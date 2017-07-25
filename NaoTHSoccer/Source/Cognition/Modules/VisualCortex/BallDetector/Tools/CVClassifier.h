/**
* @file CVClassifier.h
*
* Sorted list evaluated non-overlaping patches
*/

#ifndef _CVClassifier_H_
#define _CVClassifier_H_

#include "Tools/naoth_opencv.h"
#include "Representations/Perception/BallCandidates.h"
#include "Representations/Infrastructure/CameraInfo.h"

class CVClassifier
{
private:
  cv::Ptr<cv::ml::SVM> histModelBottom;
  cv::Ptr<cv::ml::SVM> histModelTop;

  cv::Ptr<cv::ml::SVM> loadModel(const std::string& path) const
  {
    cv::Ptr<cv::ml::SVM> histModel;
    try
    {
      histModel = cv::Algorithm::load<cv::ml::SVM>(path);
      assert(histModel->getSupportVectors().rows > 0);
      assert(histModel->isTrained());
      assert(histModel->isClassifier());
    }
    catch(cv::Exception ex)
    {
      // ignore
      std::cerr << "Could not load " << path << std::endl;
    }
    return histModel;
  }

public:
  CVClassifier() {
    histModelTop = loadModel("Config/model_histo_top.dat");
    histModelBottom = loadModel("Config/model_histo_bottom.dat");
  }


  bool classify(const BallCandidates::Patch& p, naoth::CameraInfo::CameraID cameraId)
  {
    cv::Ptr<cv::ml::SVM>& histModel = cameraId == naoth::CameraInfo::Top ? histModelTop : histModelBottom;
    assert(histModel && !histModel->empty());

    // TODO: magic numbers
    cv::Mat wrappedImg(12, 12, CV_8UC1, (void*) p.data.data());
    cv::Mat in = createHistoFeat(wrappedImg);

    cv::Mat out;
    histModel->predict(in, out, 0);

    return (out.at<float>(0,0) == 1.0f);
  }

  cv::Mat createHistoFeat(cv::Mat origImg)
  {
    cv::Mat img;
    // TODO: necessary?
    origImg.convertTo(img, CV_32FC1);

    double maxVal;
    cv::minMaxLoc(img, nullptr, &maxVal);
    maxVal += 1;

    // calculate histogram
    cv::Mat hist;
    std::vector<cv::Mat> imgVec;
    imgVec.push_back(img);

    std::vector<int> columnVec;
    columnVec.push_back(0);

    std::vector<int> binVec;
    binVec.push_back(32);

    std::vector<float> ranges;
    ranges.push_back(0.0f);
    ranges.push_back((float) maxVal);

    cv::calcHist(imgVec, columnVec, cv::Mat::ones(img.rows, img.cols, CV_8U), hist, binVec, ranges);

    // also normalize the histogram to range 0-1.0f
    double maxHistVal;
    cv::minMaxLoc(hist, nullptr, &maxHistVal);
    if(maxHistVal > 0)
    {
      hist = hist / maxHistVal;
    }

    cv::Mat feat;
    cv::transpose(hist, feat);
    return feat;
  }
};

#endif // _CVClassifier_H_
