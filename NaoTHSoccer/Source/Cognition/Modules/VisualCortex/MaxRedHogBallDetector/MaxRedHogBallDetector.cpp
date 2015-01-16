
/**
* @file MaxRedHogBallDetector.cpp
*
* Implementation of class MaxRedHogBallDetector
*
*/

#include "MaxRedHogBallDetector.h"

#include "Tools/DataStructures/ArrayQueue.h"
#include "Tools/Math/Geometry.h"
#include "Tools/CameraGeometry.h"

#include <Representations/Infrastructure/CameraInfoConstants.h>
#include <PlatformInterface/Platform.h>


MaxRedHogBallDetector::MaxRedHogBallDetector()
: 
  cameraID(CameraInfo::Bottom),
  dynamicThresholdY(255.0),
  possibleModells(570),
  dtreeTrained(false), 
  modelFileName("BallShirtModelTree.yml")
{
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:markPeak", "mark found maximum red peak in image", false);
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:markPeakScan", "mark the scanned points in image", false);
  
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:draw_scanlines","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:mark_best_points","",false);
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:mark_best_matching_points","",false);

  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:draw_ball_candidates","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:draw_ball_outtakes","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:draw_ball","..", false);  
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:use_brute_force","..", true);  
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:use_VU_difference","..", false);  

  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:mark_predicted","..", false);  

  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:draw_size_ball_field", "draw the size based position of the ball on field", false);

  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:mark_HOG", "..", false);
  DEBUG_REQUEST_REGISTER("Vision:MaxRedHogBallDetector:plot_HOG", "..", false);

  trainInput = cv::Mat::zeros(0, 270, CV_32FC1);
  trainOutput = cv::Mat::zeros(0, 1, CV_32SC1);

  // load initial training model
  std::ifstream fModel;
  fModel.open((Platform::getInstance().theConfigDirectory + "/" +  modelFileName).c_str());
  if(fModel.is_open())
  {
    fModel.close();
    dtree.load((Platform::getInstance().theConfigDirectory + "/" +  modelFileName).c_str());
    dtreeTrained = true;
    std::cout << "tree loaded " << std::endl;
  }  
  getDebugParameterList().add(&params);
}

MaxRedHogBallDetector::~MaxRedHogBallDetector()
{
  getDebugParameterList().remove(&params);
};


void MaxRedHogBallDetector::execute(CameraInfo::CameraID id)
{
  cameraID = id;
  CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
  getBallPercept().reset();

  double diff = getOverTimeHistogram().spanWidthEnv.y * 0.35 + 0.5;
  dynamicThresholdY = getOverTimeHistogram().maxEnv.y - diff;

  findBall();
}//end execute

bool MaxRedHogBallDetector::findMaximumRedPoint(Vector2i& peakPos)
{
  if(!getFieldPercept().valid) {
    return false;
  }

  FieldPercept::FieldPoly poly = getFieldPercept().getValidField();

  
  // find the top corner of the polygon
  int minY = 5000;
  for(int i = 0; i < poly.length ; i++)
  {
    if(poly.points[i].y < minY && poly.points[i].y >= 0) {
      minY = poly.points[i].y;
    }
  }

  // polygon is empty
  if(minY == 5000 || minY < 0) {
    return false;
  }

  Pixel pixel;
  int maxRedPeak = -1;
  poly = getFieldPercept().getValidField();
  Vector2i point;

  int stepSize = params.stepSize;
    
  if(getCameraMatrix().rotation.getYAngle() > Math::fromDegrees(40))
  {
    stepSize *= 3;
  }
  else if(getCameraMatrix().rotation.getYAngle() > Math::fromDegrees(10))
  {
    stepSize *= 2;
  }

  STOPWATCH_START("MaxRedHogBallDetector:searchMaxRed");
  for(point.y = minY; point.y < (int) getImage().height() - 3 ; point.y += stepSize) {
    for(point.x = 0; point.x < (int) getImage().width(); point.x += stepSize)
    {
      getImage().get(point.x, point.y, pixel);
      if
      (
        maxRedPeak < pixel.v && 
        poly.isInside_inline(point) && 
        !getBodyContour().isOccupied(point)       
      )
      {
        DEBUG_REQUEST("Vision:MaxRedHogBallDetector:markPeak",
          LINE_PX(ColorClasses::red, point.x-5, point.y, point.x+5, point.y);
          LINE_PX(ColorClasses::red, point.x, point.y-5, point.x, point.y+5);
        );
        maxRedPeak = pixel.v;
        peakPos = point;
      }

      DEBUG_REQUEST("Vision:MaxRedHogBallDetector:markPeakScan",
        POINT_PX(ColorClasses::red, point.x, point.y);
      );
    }
  }
  STOPWATCH_STOP("MaxRedHogBallDetector:searchMaxRed");

  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:markPeak",
    LINE_PX(ColorClasses::skyblue, peakPos.x-3, peakPos.y, peakPos.x+3, peakPos.y);
    LINE_PX(ColorClasses::skyblue, peakPos.x, peakPos.y-3, peakPos.x, peakPos.y+3);
  );

  return maxRedPeak >= 0;
}

bool MaxRedHogBallDetector::findBall () 
{

  // STEP 1: find the starting point for the search
  Vector2i start;
  if(!findMaximumRedPoint(start)) {
    return false;
  }


  // STEP 2: scan for the border points
  GradientSpiderScan spiderSearch(getImage(), cameraID);
  spiderSearch.setImageColorChannelNumber(2); // scan in the V channel
  spiderSearch.setCurrentGradientThreshold(params.gradientThreshold);
  //spiderSearch.setDynamicThresholdY(dynamicThresholdY);
  //spiderSearch.setUseAccumulatedChannels(true);
  spiderSearch.setMaxBeamLength(params.maxScanlineSteps);
  if(getGoalPostHistograms().valid)
  {
    spiderSearch.setMinChannelValue(getGoalPostHistograms().histogramV.median + getGoalPostHistograms().histogramV.sigma);
  }
  else
  {
    spiderSearch.setMinChannelValue(0);
  }
  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:use_VU_difference",
    spiderSearch.setUseVUdifference(true);
  );

  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:draw_scanlines",
    spiderSearch.setDrawScanLines(true);
  );
  
  goodPoints.clear(); // 
  badPoints.clear(); // 
  bestPoints.clear(); //

  STOPWATCH_START("MaxRedHogBallDetector:spiderScan");
  // explore the border of the ball
  spiderSearch.scan(start, goodPoints, badPoints);
  STOPWATCH_STOP("MaxRedHogBallDetector:spiderScan");

  // no good points found
  if(goodPoints.length <= 0) {
    return false;
  }

  //HACK: reject bad scans
/* if(goodPoints.length + badPoints.length <= 13) {
    return false;
  }*/

  //std::vector<int> featureVector;

  SimpleHogFeatureDetector detHog;

  stringstream str;

  Vector2i meanGP;

  vector<int> distances;

  STOPWATCH_START("MaxRedHogBallDetector:getHogFeatures");
  for (int i = 0; i < goodPoints.length; i++)
  {
    bestPoints.add(goodPoints[i]);
    //SimpleHogFeature hog(getImage(), goodPoints[i]);
    if(i % 2 == 1)
    {
      detHog.add(getImage(), goodPoints[i]);
      distances.push_back((start - goodPoints[i]).abs2());
      DEBUG_REQUEST_GENERIC("Vision:MaxRedHogBallDetector:plot_HOG",
        std::stringstream st;
        st << "MaxRedHogBallDetector:" << cameraID << ":HOG_" << i; 
        size_t  ii = detHog.hogVector.size() - 1;
        for(unsigned j = 0; j < detHog.hogVector[ii].size(); j++)
        {
          PLOT_GENERIC(st.str(), j, detHog.hogVector[ii][j]);
        }
      );
      DEBUG_REQUEST_GENERIC("Vision:MaxRedHogBallDetector:mark_HOG",
        size_t  ii = detHog.hogVector.size() - 1;
        for(unsigned j = 0; j < detHog.hogVector[ii].size(); j++)
        {
          double l = detHog.hogVector[ii][j];// * 0.001;
          if(l != 0)
          {
            l = log10(l) * 4;
          }
          Vector2d d(l , 0.0);
          d.rotate(((double)j)*Math::pi2/9.0 - Math::pi + Math::pi_2);

          LINE_PX(ColorClasses::pink, goodPoints[i].x, goodPoints[i].y, goodPoints[i].x + (int)(d.x+0.5), goodPoints[i].y + (int)(d.y+0.5) );
          POINT_PX(ColorClasses::green, goodPoints[i].x, goodPoints[i].y);
        }
      );

    }
    meanGP += goodPoints[i];
  }
  meanGP /= goodPoints.length;

  ////SimpleHogFeature hog(getImage(), start);
  //detHog.add(getImage(), start);
  //distances.push_back(0);

  //SimpleHogFeature hog(getImage(), meanGP);
  detHog.add(getImage(), meanGP);
  distances.push_back((start - meanGP).abs2());
  STOPWATCH_STOP("MaxRedHogBallDetector:getHogFeatures");

  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:plot_HOG",
    std::stringstream st;
    st << "MaxRedHogBallDetector:" << cameraID << ":HOG_meanGP"; 
    size_t  ii = detHog.hogVector.size() - 1;
    for(unsigned j = 0; j < detHog.hogVector[ii].size(); j++)
    {
      PLOT_GENERIC(st.str(), j, detHog.hogVector[ii][j]);
    }

    st.str("");
    st << "MaxRedHogBallDetector:" << cameraID << ":HOG_FeatureVector"; 
    for (unsigned i = 0; i < detHog.hogVector.size(); i++)
    {
      for(unsigned j = 0; j < detHog.hogVector[i].size(); j++)
      {
        PLOT_GENERIC(st.str(), i * detHog.hogVector.size() + j, detHog.hogVector[i][j]);
      }
    }
  );

  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:mark_HOG",
    size_t  ii = detHog.hogVector.size() - 1;
    for(unsigned j = 0; j < detHog.hogVector[ii].size(); j++)
    {
      double l = detHog.hogVector[ii][j];// * 0.001;
      if(l != 0)
      {
        l = log10(l) * 4;
      }
      Vector2d d(l , 0.0);
      d.rotate(((double)j)*Math::pi2/9.0 - Math::pi + Math::pi_2);

      LINE_PX(ColorClasses::pink, meanGP.x, meanGP.y, meanGP.x + (int)(d.x+0.5), meanGP.y + (int)(d.y+0.5) );
      POINT_PX(ColorClasses::green, meanGP.x, meanGP.y);
    }
  );


  int vectorLength = 103;//170;//323;// 2 * detHog.hogVector.size() * detHog.hogVector[0].size() + distances.size();
  cv::Mat currentFeature = cv::Mat::zeros(1, vectorLength, CV_32FC1);


  //int sV[270] = {   2, 7, 8, 0, 0, 11, 0, 2, 6 , 11, 3, 9, 8, 5, 0, 0, 0, 0, 
  //                  0, 6, 7, 0, 0, 11, 0, 7, 5 , 14, 4, 6, 4, 8, 0, 0, 0, 0 ,
  //                  2, 5, 2, 0, 2, 14, 6, 4, 1 , 18, 1, 3, 6, 7, 1, 0, 0, 0 ,
  //                  6, 4, 0, 0, 7, 19, 0, 0, 0 , 11, 0, 0, 0,10,15, 0, 0, 0 ,
  //                 10, 3, 0, 2, 0, 15, 3, 0, 3 , 11, 0, 5, 5,13, 2, 0, 0, 0 ,
  //                  1, 6, 6, 3, 0, 12, 2, 5, 1 , 13, 3, 5, 9, 5, 1, 0, 0, 0 ,
  //                 11, 6, 2, 2, 3, 11, 0, 0, 1 , 11, 0,14, 7, 4, 0, 0, 0, 0 ,
  //                  7, 7, 5, 1, 4, 11, 0, 0, 1 , 15, 1, 9, 4, 7, 0, 0, 0, 0 ,
  //                  3,10, 7, 0, 1, 11, 0, 0, 4 , 13, 2, 9, 7, 5, 0, 0, 0, 0 ,
  //                  0, 0, 8,14, 0, 12, 1, 1, 0 , 16, 0, 3, 7,10, 0, 0, 0, 0 ,
  //                  4,10, 6, 0, 2, 11, 0, 0, 3 , 13, 3, 8, 6, 6, 0, 0, 0, 0 ,
  //                  8,10, 0, 2, 2, 11, 1, 1, 1 , 12, 6, 7,10, 1, 0, 0, 0, 0 ,
  //                  2, 8, 6, 2, 3, 11, 0, 1, 3 , 11, 1, 7, 9, 8, 0, 0, 0, 0 ,
  //                  0, 6, 2, 0, 3, 24, 1, 0, 0 , 11, 0, 4, 6, 7, 8, 0, 0, 0 ,
  //                  6, 0, 5, 3, 1, 16, 3, 0, 2 , 11, 0, 3, 5,14, 3, 0, 0, 0 };
  //double sP = 0;
  int idx = 0;
  //std::cout << std::endl << "-------" << goodPoints.length + badPoints.length << std::endl << std::endl;

  //for (unsigned int i = 0; i < distances.size(); i++)
  //{
  //  currentFeature.at<float>(0, idx) = (float) distances[i];
  //  idx++;
  //}

  for(unsigned  i = 0; i < detHog.hogVector.size(); i++)
  {
    currentFeature.at<float>(0, idx) = (float) distances[i];
    idx++;
    //std::cout << "[ ";
    for(unsigned j = 0; j < detHog.hogVector[i].size();j++)
    {
      //if(j > 0)
      //   std::cout << ",";
      //if(detHog.hogVector[i][j] < 10)
      //  std::cout << " ";
      //std::cout << detHog.hogVector[i][j];
      //featureVector.push_back(detHog.hogVector[i][j]);
      //currentFeature.at<float>(0, idx) = (float) detHog.hogVector[i][j];
      //idx++;
      //currentFeature.at<float>(0, idx) = (float) detHog.hoiVector[i][j];
      idx++;
      //sP += sV[idx++] * detHog.hogVector[i][j];      
    }

  ////  std::cout << " # ";
  //  for(unsigned j = 0; j < detHog.hoiVector[i].size();j++)
  //  {
  //    //if(j > 0)
  //    //   std::cout << ",";
  //    //if(detHog.hoiVector[i][j] < 10)
  //    //  std::cout << " ";
  //    //std::cout << detHog.hoiVector[i][j];
  //    ////featureVector.push_back(detHog.hoiVector[i][j]);
  //    currentFeature.at<float>(0, idx) = (float) detHog.hoiVector[i][j];
  //    idx++;
  //    //sP += sV[idx++] * detHog.hoiVector[i][j];
  //  }
  ////  std::cout << " ]" << std::endl;
  }
    
  
  //double prec = sP / 72900.0;
  //std::cout << "value: " << prec << std::endl;

  //if(prec < 0.12)
  //{
  //  CIRCLE_PX(ColorClasses::red, meanGP.x, meanGP.y, 20);
  //}
  //else
  //{
  //  CIRCLE_PX(ColorClasses::green, meanGP.x, meanGP.y, 20);
  //}

  bool train = true;
  //train = false;

  if(!dtreeTrained && train)
  {
    STOPWATCH_START("MaxRedHogBallDetector:training");
    FrameInfo fI = getFrameInfo();
    //std::cout << "frame: " << fI.getFrameNumber() << std::endl;
    if(fI.getFrameNumber() < 150)
    {
      if(cameraID == CameraInfo::Top)
      {
        cv::Mat currentOutput = cv::Mat::zeros(1,1, CV_32SC1);
        trainOutput.push_back(currentOutput);
        trainInput.push_back(currentFeature);
      }
      else
      {
        cv::Mat currentOutput = cv::Mat::ones(1,1, CV_32SC1);
        trainOutput.push_back(currentOutput);
        trainInput.push_back(currentFeature);
      }

    }
    if(getFrameInfo().getFrameNumber() == 150)
    {
      cv::Mat varType = cv::Mat::ones(0,0,CV_8UC1);
      try
      {
        dtree.train(trainInput, CV_ROW_SAMPLE, trainOutput, cv::Mat(), cv::Mat(), varType);
        dtreeTrained = true;
        std::cout << "trained" << std::endl;

        // save
        dtree.save((Platform::getInstance().theConfigDirectory + "/" +  modelFileName).c_str());
        std::cout << "saved model" << std::endl;
      }
      catch(cv::Exception ex)
      {
        std::cout << "exception occured: " << ex.err << std::endl;
      }
    }
    STOPWATCH_STOP("MaxRedHogBallDetector:training");
  }

  if(dtreeTrained)
  {
    try
    {
      STOPWATCH_START("MaxRedHogBallDetector:predicting");
      CvDTreeNode* node = dtree.predict(currentFeature);
      STOPWATCH_STOP("MaxRedHogBallDetector:predicting");
      if(node != NULL)
      {
        int class_idx = node->class_idx;

        DEBUG_REQUEST("Vision:MaxRedHogBallDetector:mark_predicted",
          if(class_idx == 0)
          {
            CIRCLE_PX(ColorClasses::blue, meanGP.x, meanGP.y, 25);
            std::cout << "Ball " << cameraID << std::endl;
          }
          else if(class_idx == 1)
          {
            CIRCLE_PX(ColorClasses::pink, meanGP.x, meanGP.y, 25);
            std::cout << "Shirt "  << cameraID << std::endl;
          }
        );
      }
    }
    catch(cv::Exception ex)
    {
      std::cout << ex.msg << std::endl;
    }
  }

  //  5 11|5 11|5 15|5 14|3 13|2 12|0 12|5 12|5 12|3 20|5 12|5 11|5 11|5 16|3 16
  //# 3 11|3 12|0 12|5 11|3 16|0 13|3 17|0 15|0 13|3 12|0 14|0 13|0 12|0 12|0 11
  int sampleVector[60] = {5, 11, 5, 11, 5, 12, 5, 12, 2, 13, 5, 11, 0, 12, 1, 12, 5, 11, 5, 14, 5, 11, 0, 12, 5, 11, 5, 14, 3, 20,
                0, 14, 0, 14, 0, 18, 5, 15, 0, 20, 0, 12, 3, 12, 0, 12, 0, 12, 2, 12, 2, 12, 0, 12, 0, 12, 0, 12, 0, 12};
  int sPVector[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  double scalarProduct = 0;

  //std::cout << "{ ";
  //for(unsigned j = 0; j < detHog.maxGradiantVector.size(); j++)
  //{
  //  if(j > 0)
  //      std::cout << ",";
  //  std::cout << detHog.maxGradiantVector[j];
  //  scalarProduct += sampleVector[2 * j] * detHog.maxGradiantVector[j].x;
  //  //scalarProduct += sampleVector[2 * j + 1] * detHog.maxGradiantVector[j].y;

  //}
  //std::cout << std::endl << " # ";
  //for(unsigned j = 0; j < detHog.maxIntensityVector.size();j++)
  //{
  //  if(j > 0)
  //      std::cout << ",";
  //  std::cout << detHog.maxIntensityVector[j];
  //  scalarProduct += sampleVector[30 + 2 * j] * detHog.maxIntensityVector[j].x;
  //  //scalarProduct += sampleVector[30 + 2 * j + 1] * detHog.maxIntensityVector[j].y;
  //}
  //double outcome = scalarProduct / 900.0;//3600.0;
  //std::cout << " }" << std::endl;
  //std::cout << "value: " << outcome << std::endl;




  GT_TRACE("MaxRedHogBallDetector:4");
  clearDublicatePoints(bestPoints);

  // display the final points
  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:mark_best_points",
    for(int i = 0; i < bestPoints.length; i++) {
      PEN("FF0000", 0.1);
      LINE(bestPoints[i].x-1, bestPoints[i].y, bestPoints[i].x+1, bestPoints[i].y);
      LINE(bestPoints[i].x, bestPoints[i].y-1, bestPoints[i].x, bestPoints[i].y+1);
    }
  );

  GT_TRACE("MaxRedHogBallDetector:5");
  // STEP 3: try to match a ball model
  return getBestModel(bestPoints, start);
}

bool MaxRedHogBallDetector::getBestModel(const BallPointList& pointList, const Vector2i& start)
{
  STOPWATCH_START("MaxRedHogBallDetector:bestModel");
  
  GT_TRACE("MaxRedHogBallDetector:2");
  if(pointList.length < 3) {
    return false;
  }
  
  Vector2d centerBest;
  double radiusBest;

  bool bestModelFound = false;
  bool useBruteForce = false;

  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:use_brute_force",
    useBruteForce = true;
  );
  
  if(useBruteForce) {
    bestModelFound = getBestBallBruteForce(pointList, start, centerBest, radiusBest);
  } else {
    bestModelFound = getBestBallRansac(pointList, start, centerBest, radiusBest);
  }

  // calculate the percept
  if(bestModelFound)
  {
    bool projectionOK = CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), 
      getImage().cameraInfo,
      centerBest.x, 
      centerBest.y, 
      getFieldInfo().ballRadius,
      getBallPercept().bearingBasedOffsetOnField);

    // HACK: don't take to far balls
    projectionOK = projectionOK && getBallPercept().bearingBasedOffsetOnField.abs2() < 10000*10000; // closer than 10m

    // HACK: if the ball center is in image it has to be in the field polygon 
    Vector2d ballPointToCheck(centerBest.x, centerBest.y - radiusBest);
    projectionOK = projectionOK && 
      (getImage().isInside((int)(ballPointToCheck.x+0.5), (int)(ballPointToCheck.y+0.5)) && 
       getFieldPercept().getValidField().isInside(ballPointToCheck));

    STOPWATCH_STOP("MaxRedHogBallDetector:bestModel");

    if(projectionOK) 
    {
      getBallPercept().radiusInImage = radiusBest;
      getBallPercept().centerInImage = centerBest;
      getBallPercept().ballWasSeen = projectionOK;
      getBallPercept().frameInfoWhenBallWasSeen = getFrameInfo();
    
      DEBUG_REQUEST("Vision:MaxRedHogBallDetector:draw_ball",
        PEN("FFFF00", 0.1);
        CIRCLE( (centerBest.x),  (centerBest.y), (radiusBest));    
      );
    }
    return true;
  }

  return false;
}

bool MaxRedHogBallDetector::getBestBallBruteForce(const BallPointList& pointList, const Vector2i& /*start*/, Vector2d& centerBest, double& radiusBest)
{
  int idxBest = -1;
  int bestCount = 0;
  Vector2d center;
  double radius = 0;
  double bestErr = 10000.0;
  Vector2i boundingBoxMin(getImage().width() - 1, getImage().height() - 1);
  Vector2i boundingBoxMax;
    GT_TRACE("MaxRedHogBallDetector:1");
  // initialize the first model with all avaliable points
  possibleModells[0].clear();
/*  for(int j = 0; j < pointList.length; j++) 
  {
    possibleModells[0].add(pointList[j]);

    boundingBoxMin.x = min(boundingBoxMin.x, pointList[j].x);
    boundingBoxMin.y = min(boundingBoxMin.y, pointList[j].y);
    boundingBoxMax.x = max(boundingBoxMax.x, pointList[j].x);
    boundingBoxMax.y = max(boundingBoxMax.y, pointList[j].y);
  }

  double diag2 = (boundingBoxMax - boundingBoxMin).abs2();*/

  int firstPoint = -1;
  int  secondPoint = 1;
  int  thirdPoint = 2;    
      
  BallPointList list;

  for(size_t i = 1; i < possibleModells.size(); i++)
  {
    firstPoint++;
    if (firstPoint == secondPoint)
    {
      firstPoint = 0;
      secondPoint++;
    }
    if (secondPoint == thirdPoint) 
    {
      secondPoint = 1;
      thirdPoint++;
    }
    if (thirdPoint == pointList.length) 
    {
      break;
    }

    possibleModells[i].clear();    
    possibleModells[i].add(pointList[firstPoint]);
    possibleModells[i].add(pointList[secondPoint]);
    possibleModells[i].add(pointList[thirdPoint]);

    if(Geometry::calculateCircle(possibleModells[i], center, radius))
    {  
      //has the ballModel an acceptable radius?
      if (radius < params.minRadiusInImage || radius > params.maxRadiusInImage) continue;
      //check if ballModel contains green then this is no ball
      if (checkBallForGreen(center, radius)) continue;      
      DEBUG_REQUEST("Vision:MaxRedHogBallDetector:draw_ball_candidates",
        PEN("0000FF", 0.1);        
        CIRCLE(center.x, center.y, radius);
      );
      // calculate the number of inliers      
      double radiusErrMax =  params.ransacPercentValid * radius;
      int count = 0;
      double meanError = 0.0;
      list.clear();
      for(int j = 0; j < pointList.length; j++)
      {
        double err = fabs((center - Vector2d(pointList[j])).abs() - radius);
        if(err <= radiusErrMax) {
          count++;
          list.add(pointList[j]);
        }
        meanError += err;
      }

      if(pointList.length > 0) {
        meanError /= (double)(pointList.length);
      }

      if(params.minPercentOfPointsUsed  >= (double) count/ (double) pointList.length &&
          (count > bestCount || (count == bestCount && meanError < bestErr) )// &&
         // (Vector2d(start) - center).abs2() <= radius*radius //&&
             //diag2 > radius * radius
             )
      {        
        centerBest = center;
        radiusBest = radius;
        bestCount = count;
        bestErr = meanError;
        idxBest = (int) i;
        bestMatchedBallpoints.clear();
        for(int j = 0; j < list.length; j++)
        {
          bestMatchedBallpoints.add(list[j]);
        }
      }
    }
  }


  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:mark_best_matching_points",
    if(idxBest >= 0)
    {
      drawUsedPoints(bestMatchedBallpoints);
    }
  );

  return idxBest >= 0;
}

bool MaxRedHogBallDetector::getBestBallRansac(const BallPointList& pointList, const Vector2i& start, Vector2d& centerBest, double& radiusBest)
{
  int maxTries = min(pointList.length, params.maxRansacTries);
  int idxBest = -1;
  int bestCount = 0;
  Vector2d center;
  double radius = 0;
  double bestErr = 10000.0;
  BallPointList list;
  Vector2i boundingBoxMin(getImage().width() - 1, getImage().height() - 1);
  Vector2i boundingBoxMax;

  // initialize the first model with all avaliable points
  possibleModells[0].clear();
  for(int j = 0; j < pointList.length; j++) 
  {
    possibleModells[0].add(pointList[j]);
    boundingBoxMin.x = min(boundingBoxMin.x, pointList[j].x);
    boundingBoxMin.y = min(boundingBoxMin.y, pointList[j].y);
    boundingBoxMax.x = max(boundingBoxMax.x, pointList[j].x);
    boundingBoxMax.y = max(boundingBoxMax.y, pointList[j].y);
  }

  double diag2 = (boundingBoxMax - boundingBoxMin).abs2();

  for(int i = 1; i < maxTries && i < (int)possibleModells.size(); i++)
  {
    int idx1 = Math::random(pointList.length);
    int idx2 = Math::random(pointList.length);
    int idx3 = Math::random(pointList.length);

    for (int j=0;j<maxTries && (idx1==idx2 || idx2==idx3 || idx1==idx3); j++) 
    {
      idx1 = Math::random(pointList.length);
      idx2 = Math::random(pointList.length);
      idx3 = Math::random(pointList.length);
    }

    possibleModells[i].clear();

    if(idx1==idx2 || idx2==idx3 || idx1==idx3) {
      continue;
    }

    possibleModells[i].add(pointList[idx1]);
    possibleModells[i].add(pointList[idx2]);
    possibleModells[i].add(pointList[idx3]);

    if(Geometry::calculateCircle(possibleModells[i], center, radius))
    {
      DEBUG_REQUEST("Vision:MaxRedHogBallDetector:draw_ball_candidates",
        PEN("0000FF", 0.1);
        CIRCLE((int) center.x+0.5, (int) center.y+0.5, (int) radius+0.5);
      );

      // calculate the number of inliers
      double radiusErrMax =  params.ransacPercentValid * radius;
      int count = 0;
      double meanError = 0.0;
      list.clear();
      for(int j = 0; j < pointList.length; j++)
      {
        double err = fabs((center - Vector2d(pointList[j])).abs() - radius);
        if(err <= radiusErrMax) {
          count++;
          list.add(pointList[j]);
        }
        meanError += err;
      }

      if(pointList.length > 0) {
        meanError /= (double)(pointList.length);
      }

      if( count*2 >= pointList.length && 
          (count > bestCount || (count == bestCount && meanError < bestErr) ) &&
          (Vector2d(start) - center).abs2() <= radius*radius &&
          radius >= params.minRadiusInImage && radius <= params.maxRadiusInImage &&
          diag2 > radius * radius 
      )
      {
        centerBest = center;
        radiusBest = radius;
        bestCount = count;
        bestErr = meanError;
        idxBest = i;
        bestMatchedBallpoints.clear();
        for(int j = 0; j < list.length; j++)
        {
          bestMatchedBallpoints.add(list[j]);
        }
      }
    }
  }

  DEBUG_REQUEST("Vision:MaxRedHogBallDetector:mark_best_matching_points",
    if(idxBest >= 0)
    {
      drawUsedPoints(bestMatchedBallpoints);
    }
  );

  return idxBest >= 0;
}

void MaxRedHogBallDetector::drawUsedPoints(const BallPointList& pointList)
{
  for(int i = 0; i < pointList.length; i++)
  {
    LINE_PX(ColorClasses::green, pointList[i].x-2, pointList[i].y, pointList[i].x-2, pointList[i].y);
    LINE_PX(ColorClasses::green, pointList[i].x, pointList[i].y-2, pointList[i].x, pointList[i].y+2);
  }
}

Vector2i MaxRedHogBallDetector::getCenterOfMass (BallPointList& pointList) 
{
   Vector2d mean;
  ASSERT(pointList.length > 0);
  for (int i =0; i< pointList.length; i++)
  {
    mean += pointList[i];
  }
  mean /= pointList.length;
  return mean;
}

bool MaxRedHogBallDetector::checkBallForGreen(Vector2d center, double radius) {
  Pixel pixel;
  Vector2i point;
  point.x =(int) center.x;
  point.y =(int) center.y + (int)(radius * params.checkBallForGreen + 0.5);
  setToLastPointInImage(point);
  getImage().get(point.x, point.y, pixel);
  if (getFieldColorPercept().isFieldColor(pixel)){
    return true;
  }
  point.x =(int) center.x;
  point.y =(int) center.y - (int)(radius * params.checkBallForGreen + 0.5);
  setToLastPointInImage(point);
  getImage().get(point.x, point.y, pixel);
  if (getFieldColorPercept().isFieldColor(pixel)){
    return true;
  }
  point.x =(int) center.x + (int)(radius * params.checkBallForGreen + 0.5);;
  point.y =(int) center.y; 
  setToLastPointInImage(point);
  getImage().get(point.x, point.y, pixel);
  if (getFieldColorPercept().isFieldColor(pixel)){
    return true;
  }
  point.x =(int) center.x - (int)(radius * params.checkBallForGreen + 0.5);;
  point.y =(int) center.y; 
  setToLastPointInImage(point);
  getImage().get(point.x, point.y, pixel);
  if (getFieldColorPercept().isFieldColor(pixel)){
    return true;
  }
  return false;

}

void MaxRedHogBallDetector::setToLastPointInImage(Vector2i& point) {
  if (point.x < 0 ) point.x = 0;
  else if (point.x >= (int) getImage().width()) point.x = getImage().width()-1;
  if (point.y < 0 ) point.y = 0;
  else if (point.y >=(int) getImage().height()) point.y = getImage().height()-1;
}

void MaxRedHogBallDetector::clearDublicatePoints (BallPointList& ballPoints) 
{
  vector<Vector2i > dublicatePointList;
  
  for (int i=0;i<ballPoints.length; i++) 
  {
    bool foundDublicate = false;
    for(int j=i+1; j<ballPoints.length; j++) 
    {
      if (ballPoints[i]==ballPoints[j]) {
        foundDublicate = true;
        break;
      }
    }
    if (!foundDublicate) {
      dublicatePointList.push_back(ballPoints[i]);
    }
  }

  if ((int)dublicatePointList.size() < ballPoints.length) 
  {
    ballPoints.clear();
    for (size_t i=0; i < dublicatePointList.size(); i++) {
      ballPoints.add(dublicatePointList[i]);      
    }
  }
}

