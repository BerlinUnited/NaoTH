#include "FieldSideDetector.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/ImageProcessing/BresenhamLineScan.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include <DebugCommunication/DebugCommandManager.h>

FieldSideDetector::FieldSideDetector()
  : dtreeTrained(false)
{
  DEBUG_REQUEST_REGISTER("FieldSideDetector:scanlines", "Show the scanlines", false);
  DEBUG_REQUEST_REGISTER("FieldSideDetector:own_goal", "Record own goal", false);
  DEBUG_REQUEST_REGISTER("FieldSideDetector:opponent_goal", "Record opponent goal", false);

  DEBUG_REQUEST_REGISTER("FieldSideDetector:show_classification_result", "Show the classification result", false);

  REGISTER_DEBUG_COMMAND("train_fieldside", "Train the FieldSideDetector", this);
  REGISTER_DEBUG_COMMAND("clear_fieldside", "Clear the FieldSideDetector training data", this);

  trainInput = cv::Mat::zeros(0, 6, CV_32FC1);
  trainOutput = cv::Mat::zeros(0, 1, CV_32SC1);
}

void FieldSideDetector::execute()
{
  getFieldSidePercept().facedFieldSide = FieldSidePercept::unknown;

  const GoalPercept& goalPercept = getGoalPercept();

  if(goalPercept.getNumberOfSeenPosts() > 1)
  {
    Vector2<double> h1(getCameraMatrix().horizon.begin());
    Vector2<double> h2(getCameraMatrix().horizon.end());
    int heightOfHorizon = (int)((h1.y + h2.y) * 0.5 + 0.5);

    const GoalPercept::GoalPost& postLeft =
        goalPercept.getPost(0).type == GoalPercept::GoalPost::leftPost
        ? goalPercept.getPost(0) : goalPercept.getPost(1);
    const GoalPercept::GoalPost& postRight =
        goalPercept.getPost(1).type == GoalPercept::GoalPost::rightPost
        ? goalPercept.getPost(1) : goalPercept.getPost(0);

    if(postLeft.topPoint.y < heightOfHorizon && postRight.topPoint.y < heightOfHorizon)
    {
      cv::Mat currentFeature = cv::Mat::zeros(1, 4, CV_32FC1);

      BresenhamLineScan scanLine0(postLeft.basePoint, postRight.topPoint);
      BresenhamLineScan scanLine1(postRight.basePoint, postLeft.topPoint);

      double sumY_0 = 0.0;
      double sumV_0 = 0.0;
      double sumU_0 = 0.0;

      double sumY_1 = 0.0;
      double sumV_1 = 0.0;
      double sumU_1 = 0.0;

      Vector2<int> p = postLeft.basePoint;
      for(int s=0; s < scanLine0.numberOfPixels; s++)
      {
        scanLine0.getNext(p);
        Pixel color = getImage().get(p.x, p.y);
        sumY_0 += color.y;
        sumU_0 += color.u;
        sumV_0 += color.v;

        DEBUG_REQUEST("FieldSideDetector:scanlines",
          POINT_PX(ColorClasses::blue, p.x, p.y);
        );
      }

      p = postRight.basePoint;
      for(int s=0; s < scanLine1.numberOfPixels; s++)
      {
        scanLine1.getNext(p);
        Pixel color = getImage().get(p.x, p.y);
        sumY_1 += color.y;
        sumU_1 += color.u;
        sumV_1 += color.v;

        DEBUG_REQUEST("FieldSideDetector:scanlines",
          POINT_PX(ColorClasses::red, p.x, p.y);
        );
      }

      if(scanLine0.numberOfPixels > 0 && scanLine1.numberOfPixels > 0)
      {
        double meanY_0 = sumY_0 / (double) scanLine0.numberOfPixels;
        double meanU_0 = sumU_0 / (double) scanLine0.numberOfPixels;
        double meanV_0 = sumV_0 / (double) scanLine0.numberOfPixels;

        double meanY_1 = sumY_1 / (double) scanLine0.numberOfPixels;
        double meanU_1 = sumU_1 / (double) scanLine1.numberOfPixels;
        double meanV_1 = sumV_1 / (double) scanLine1.numberOfPixels;

        currentFeature.at<float>(0,0) = meanY_0;
        currentFeature.at<float>(0,1) = meanU_0;
        currentFeature.at<float>(0,2) = meanV_0;

        currentFeature.at<float>(0,3) = meanY_1;
        currentFeature.at<float>(0,4) = meanU_1;
        currentFeature.at<float>(0,5) = meanV_1;

        PLOT("FieldSideDetector:mean_Y_0", meanY_0);
        PLOT("FieldSideDetector:mean_U_0", meanU_0);
        PLOT("FieldSideDetector:mean_V_0", meanV_0);

        PLOT("FieldSideDetector:mean_Y_1", meanY_1);
        PLOT("FieldSideDetector:mean_U_1", meanU_1);
        PLOT("FieldSideDetector:mean_V_1", meanV_1);

        DEBUG_REQUEST("FieldSideDetector:own_goal",
          cv::Mat currentOutput = cv::Mat::zeros(1,1, CV_32SC1);
          trainOutput.push_back(currentOutput);
          trainInput.push_back(currentFeature);
        );
        DEBUG_REQUEST("FieldSideDetector:opponent_goal",
          cv::Mat currentOutput = cv::Mat::ones(1,1, CV_32SC1);
          trainOutput.push_back(currentOutput);
          trainInput.push_back(currentFeature);
        );
      }

      if(dtreeTrained)
      {
        CvDTreeNode* node = dtree.predict(currentFeature);
        if(node != NULL)
        {
          int class_idx = node->class_idx;

          if(class_idx == 0)
          {
            getFieldSidePercept().facedFieldSide = FieldSidePercept::own;
          }
          else if(class_idx == 1)
          {
            getFieldSidePercept().facedFieldSide = FieldSidePercept::opponent;
          }

          DEBUG_REQUEST("FieldSideDetector:show_classification_result",
            ColorClasses::Color c = class_idx == 0 ? ColorClasses::blue : ColorClasses::pink;
            RECT_PX(c, 0, 0, 20, 20);
            LINE_PX(c, 0,0, 20, 20);
            LINE_PX(c, 0, 20, 20, 0);
          );
        } // end if node not null
      } // end if trained once
    } // end if top points over horizon
  } // end if goalPercepts has more than 1 post
}

void FieldSideDetector::executeDebugCommand(const std::string &command,
                                            const std::map<std::string, std::string> &arguments,
                                            std::ostream &outstream)
{
  if(command == "train_fieldside")
  {
    if(trainInput.rows == 0)
    {
      outstream << "no goal shown yet, will not train" << std::endl;
    }
    else
    {
      cv::Mat varType = cv::Mat::ones(0,0,CV_8UC1);
      dtree.train(trainInput, CV_ROW_SAMPLE, trainOutput, cv::Mat(), cv::Mat(), varType);
      dtreeTrained = true;
      outstream << "trained" << std::endl;
    }
  }
  else if(command == "clear_fieldside")
  {
    trainInput = cv::Mat::zeros(0, 6, CV_32FC1);
    trainOutput = cv::Mat::zeros(0, 1, CV_32SC1);
    outstream << "cleared" << std::endl;
  }
}

FieldSideDetector::~FieldSideDetector()
{
}
