#include "BaseColorClassifier.h"

BaseColorClassifier::BaseColorClassifier()
:
  bPercept(getBaseColorRegionPercept()),
  cTable(getColorTable64()),
  fPercept(getFieldColorPercept()),
  histogram(getHistogram()),
  coloredGrid(getColoredGrid())
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_ball_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_lines_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_field_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_ball", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_field", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_lines", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:field_y", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:field_u", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:field_v", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_y", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_u", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_v", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_y", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_u", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_v", " ", false);

  lastMeanY = coloredGrid.meanBrightness;
  lastMeanU = coloredGrid.meanBlue;
  lastMeanV = coloredGrid.meanRed;
  adaptationRate = 0.9;
  initPercept();
  goalIsCalibrating = false;
}

void BaseColorClassifier::initPercept()
{
  bPercept.fieldCalibIndex = regionParams.fieldIndex;
  bPercept.fieldIndex = regionParams.fieldIndex;
  bPercept.fieldCalibDist = regionParams.fieldDist;
  bPercept.fieldDist = regionParams.fieldDist;

  bPercept.goalCalibIndex = regionParams.goalIndex;
  bPercept.goalIndex = regionParams.goalIndex;
  bPercept.goalCalibDist = regionParams.goalDist;
  bPercept.goalDist = regionParams.goalDist;

  bPercept.ballCalibIndex = regionParams.ballIndex;
  bPercept.ballIndex = regionParams.ballIndex;
  bPercept.ballCalibDist = regionParams.ballDist;
  bPercept.ballDist = regionParams.ballDist;

  bPercept.lineCalibIndex = regionParams.lineIndex;
  bPercept.lineIndex = regionParams.lineIndex;
  bPercept.lineCalibDist = regionParams.lineDist;
  bPercept.lineDist = regionParams.lineDist;
  bPercept.setPerceptRegions();
}

void BaseColorClassifier::execute()
{
  bPercept.meanImg.y = coloredGrid.meanBrightness;
  bPercept.meanImg.u = coloredGrid.meanBlue;
  bPercept.meanImg.v = coloredGrid.meanRed;

  double diff = fabs(coloredGrid.meanBrightness - lastMeanY);
  if(diff > 5.0)
  {
    meanY.add(coloredGrid.meanBrightness);
    bPercept.meanEnv.y = meanY.getAverage();
    bPercept.diff.y = diff;
    lastMeanY = coloredGrid.meanBrightness;
  }
  diff = fabs(coloredGrid.meanBlue - lastMeanU);
  if(diff > 5.0)
  {
    meanU.add(coloredGrid.meanBlue);
    bPercept.meanEnv.u = meanU.getAverage();
    bPercept.diff.u = diff;
    lastMeanU = coloredGrid.meanBlue;
  }
  diff = fabs(coloredGrid.meanRed - lastMeanV);
  if(diff > 5.0)
  {
    meanV.add(coloredGrid.meanRed);
    bPercept.meanEnv.v = meanV.getAverage();
    bPercept.diff.v = diff;
    lastMeanV = coloredGrid.meanRed;
  }

//  Vector2<double> ballBorderY;
//  Vector2<double> ballBorderU;
//  Vector2<double> ballBorderV;
//  unsigned int maxBallIndexY = 0;
//  unsigned int maxBallIndexU = 0;
//  unsigned int maxBallIndexV = 0;
////=======
////  double diff = fabs(getColoredGrid().meanBrightness - lastMeanY);
////  if(diff > 5.0)
////  {
////    meanY.add(getColoredGrid().meanBrightness);
////    getBaseColorRegionPercept().meanY = meanY.getAverage();
////    getBaseColorRegionPercept().diffY = diff;
////    lastMeanY = getColoredGrid().meanBrightness;
////  }
////  diff = fabs(getColoredGrid().meanBlue - lastMeanU);
////  if(diff > 5.0)
////  {
////    meanU.add(getColoredGrid().meanBlue);
////    getBaseColorRegionPercept().meanU = meanU.getAverage();
////    getBaseColorRegionPercept().diffU = diff;
////    lastMeanU = getColoredGrid().meanBlue;
////  }
////  diff = fabs(getColoredGrid().meanRed - lastMeanV);
////  if(diff > 5.0)
////  {
////    meanV.add(getColoredGrid().meanRed);
////    getBaseColorRegionPercept().meanV = meanV.getAverage();
////    getBaseColorRegionPercept().diffV = diff;
////    lastMeanV = getColoredGrid().meanRed;
////  }

////  unsigned int maxBallIndexY = 0;
////  unsigned int maxBallIndexU = 0;
////  unsigned int maxBallIndexV = 0;

////>>>>>>> MERGE-SOURCE
//  double maxBallY = 0.0;
//  double maxBallU = 0.0;
//  double maxBallV = 0.0;

//  unsigned int maxGoalIndexY = 0;
//  unsigned int maxGoalIndexU = 0;
//  unsigned int maxGoalIndexV = 0;
//  double maxGoalY = 0.0;
//  double maxGoalU = 0.0;
//  double maxGoalV = 0.0;

////  Vector2<double> lineBorderY;
////  Vector2<double> lineBorderU;
////  Vector2<double> lineBorderV;
////  unsigned int maxLineIndexY = 0;
////  unsigned int maxLineIndexU = 0;
////  unsigned int maxLineIndexV = 0;
////  double maxLineY = 0.0;
////  double maxLineU = 0.0;
////  double maxLineV = 0.0;

  MODIFY("BaseColorClassifier:adaptationRate", adaptationRate);

  MODIFY("BaseColorClassifier:fieldDistY", bPercept.fieldDist.y);
  MODIFY("BaseColorClassifier:fieldDistU", bPercept.fieldDist.u);
  MODIFY("BaseColorClassifier:fieldDistV", bPercept.fieldDist.v);

  MODIFY("BaseColorClassifier:fieldIndexY", bPercept.fieldIndex.y);
  MODIFY("BaseColorClassifier:fieldIndexU", bPercept.fieldIndex.u);
  MODIFY("BaseColorClassifier:fieldIndexV", bPercept.fieldIndex.v);

  MODIFY("BaseColorClassifier:goalDistY", bPercept.goalDist.y);
  MODIFY("BaseColorClassifier:goalDistU", bPercept.goalDist.u);
  MODIFY("BaseColorClassifier:goalDistV", bPercept.goalDist.v);

  MODIFY("BaseColorClassifier:goalIndexY", bPercept.goalIndex.y);
  MODIFY("BaseColorClassifier:goalIndexU", bPercept.goalIndex.u);
  MODIFY("BaseColorClassifier:goalIndexV", bPercept.goalIndex.v);

  MODIFY("BaseColorClassifier:ballDistY", bPercept.ballDist.y);
  MODIFY("BaseColorClassifier:ballDistU", bPercept.ballDist.u);
  MODIFY("BaseColorClassifier:ballDistV", bPercept.ballDist.v);

  MODIFY("BaseColorClassifier:ballIndexY", bPercept.ballIndex.y);
  MODIFY("BaseColorClassifier:ballIndexU", bPercept.ballIndex.u);
  MODIFY("BaseColorClassifier:ballIndexV", bPercept.ballIndex.v);

  MODIFY("BaseColorClassifier:lineDistY", bPercept.lineDist.y);
  MODIFY("BaseColorClassifier:lineDistU", bPercept.lineDist.u);
  MODIFY("BaseColorClassifier:lineDistV", bPercept.lineDist.v);

  MODIFY("BaseColorClassifier:lineIndexY", bPercept.lineIndex.y);
  MODIFY("BaseColorClassifier:lineIndexU", bPercept.lineIndex.u);
  MODIFY("BaseColorClassifier:lineIndexV", bPercept.lineIndex.v);

//  int reliableGoalPostCount = 0;

//  for(unsigned int p = 0; p < getGoalPercept().getNumberOfSeenPosts(); p++)
//  {
//    if(getGoalPercept().getPost(p).positionReliable)
//    {
//      reliableGoalPostCount++;
//    }
//  }


//  for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
//  {
//    if(maxBallY < histogram.colorChannelHistogramBall[0][i])
//    {
//      maxBallY = histogram.colorChannelHistogramBall[0][i];
//      maxBallIndexY = i;
//    }
//    if (maxBallY > 1.0)
//    {
//      if(maxBallIndexY == 0.0)
//      {
//        ballBorderY.x = i;
//      }
//      else
//      {
//        ballBorderY.y = i;
//      }
//    }
//    if(maxBallU < histogram.colorChannelHistogramBall[1][i])
//    {
//      maxBallU = histogram.colorChannelHistogramBall[1][i];
//      maxBallIndexU = i;
//    }
//    if (maxBallU > 1.0)
//    {
//      if(maxBallIndexU == 0.0)
//      {
//        ballBorderU.x = i;
//      }
//      else
//      {
//        ballBorderU.y = i;
//      }
//    }
//    if(maxBallV < histogram.colorChannelHistogramBall[2][i])
//    {
//      maxBallV = histogram.colorChannelHistogramBall[2][i];
//      maxBallIndexV = i;
//    }
//    if (maxBallV > 1.0)
//    {
//      if(maxBallIndexV == 0.0)
//      {
//        ballBorderV.x = i;
//      }
//      else
//      {
//        ballBorderV.y = i;
//      }
//    }

//   // if(reliableGoalPostCount >= 1)
//    {
//      //if(i >= bPercept.goalBorderMinus.y && i <= bPercept.goalBorderPlus.y)
//      {
//        if(histogram.colorChannelHistogramGoal[0][i] > 0 && maxGoalY < histogram.colorChannelHistogramGoal[0][i])
//        {
//          maxGoalY = histogram.colorChannelHistogramGoal[0][i];
//          maxGoalIndexY = i;
//        }
//      }
//      //if(i >= bPercept.goalBorderMinus.u && i <= bPercept.goalBorderPlus.u)
//      {
//        if(histogram.colorChannelHistogramGoal[1][i] > 0 && maxGoalU < histogram.colorChannelHistogramGoal[1][i])
//        {
//          maxGoalU = histogram.colorChannelHistogramGoal[1][i];
//          maxGoalIndexU = i;
//        }
//      }
//      //if(i >= bPercept.goalBorderMinus.v && i <= bPercept.goalBorderPlus.v)
//      {
//        if(histogram.colorChannelHistogramGoal[2][i] > 0 && maxGoalV < histogram.colorChannelHistogramGoal[2][i])
//        {
//          maxGoalV = histogram.colorChannelHistogramGoal[2][i];
//          maxGoalIndexV = i;
//        }
//      }
//    }
//    //    if(maxLineY < histogram.colorChannelHistogramLine[0][i])
//    //    {
//    //      maxLineY = histogram.colorChannelHistogramLine[0][i];
//    //      maxLineIndexY = i;
//    //      if (maxLineY > 1.0)
//    //      {
//    //        if(maxLineIndexY == 0.0)
//    //        {
//    //          lineBorderY.x = i;
//    //        }
//    //        else
//    //        {
//    //          lineBorderY.y = i;
//    //        }
//    //      }
//    //    }
//    //    if(maxLineU < histogram.colorChannelHistogramLine[1][i])
//    //    {
//    //      maxLineU = histogram.colorChannelHistogramLine[1][i];
//    //      maxLineIndexU = i;
//    //      if (maxLineU > 1.0)
//    //      {
//    //        if(maxLineIndexU == 0.0)
//    //        {
//    //          lineBorderU.x = i;
//    //        }
//    //        else
//    //        {
//    //          lineBorderU.y = i;
//    //        }
//    //      }
//    //    }
//    //    if(maxLineV < histogram.colorChannelHistogramLine[2][i])
//    //    {
//    //      maxLineV = histogram.colorChannelHistogramLine[2][i];
//    //      maxLineIndexV = i;
//    //      if (maxLineV > 1.0)
//    //      {
//    //        if(maxLineIndexV == 0.0)
//    //        {
//    //          lineBorderV.x = i;
//    //        }
//    //        else
//    //        {
//    //          lineBorderV.y = i;
//    //        }
//    //      }
//    //    }
//  }

//  Vector2< unsigned int> goalBorderY;
//  Vector2< unsigned int> goalBorderU;
//  Vector2< unsigned int> goalBorderV;

//  //if(reliableGoalPostCount >= 1)
//  {
//    unsigned int g = 0;
//    unsigned int bLeft = g;
//    unsigned int bRight = COLOR_CHANNEL_VALUE_COUNT - 1;
//    bool foundLeft = false;
//    bool foundRight = false;

//    while((!foundLeft || !foundRight) && g < bPercept.goalCalibDist.y && bRight >= 0 && bLeft < bRight)
//    {

//      if(!foundLeft)
//      {
//        goalBorderY.x = bLeft;
//        if(histogram.colorChannelHistogramGoal[0][bLeft] > 1)
//        {
//          foundLeft = true;
//        }
//      }
//      if(!foundRight)
//      {
//        goalBorderY.y = bRight;
//        if(histogram.colorChannelHistogramGoal[0][bRight] > 1)
//        {
//          foundRight = true;
//        }
//      }
//      g++;
//      bLeft++;
//      bRight--;
//    }

//    g = 0;
//    bLeft = maxGoalIndexU - g;
//    bRight = maxGoalIndexU + g;
//    foundLeft = false;
//    foundRight = false;

//    while((!foundLeft || !foundRight) && g < bPercept.goalCalibDist.u && bLeft >= 0 && bRight < COLOR_CHANNEL_VALUE_COUNT)
//    {

//      if(!foundLeft)
//      {
//        goalBorderU.x = bLeft;
//        if(histogram.colorChannelHistogramGoal[1][bLeft] < 1)
//        {
//          foundLeft = true;
//        }
//      }
//      if(!foundRight)
//      {
//        goalBorderU.y = bRight;
//        if(histogram.colorChannelHistogramGoal[1][bRight] < 1)
//        {
//          foundRight = true;
//        }
//      }
//      g++;
//      bLeft = maxGoalIndexU - g;
//      bRight = maxGoalIndexU + g;
//    }

//    g = 0;
//    bLeft = maxGoalIndexV - g;
//    bRight = maxGoalIndexV + g;
//    foundLeft = false;
//    foundRight = false;

//    while((!foundLeft || !foundRight) && g < bPercept.goalCalibDist.v && bLeft >= 0 && bRight < COLOR_CHANNEL_VALUE_COUNT)
//    {

//      if(!foundLeft)
//      {
//        goalBorderV.x = bLeft;
//        if(histogram.colorChannelHistogramGoal[2][bLeft] < 1)
//        {
//          foundLeft = true;
//        }
//      }
//      if(!foundRight)
//      {
//        goalBorderV.y = bRight;
//        if(histogram.colorChannelHistogramGoal[2][bRight] < 1)
//        {
//          foundRight = true;
//        }
//      }
//      g++;
//      bLeft = maxGoalIndexV - g;
//      bRight = maxGoalIndexV + g;
//    }
//  }

//  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_ball",
//    if(ballBorderY.abs2() > 0 && ballBorderU.abs2() > 0  && ballBorderV.abs2() > 0)
//    {
//      maxBallIndexY = fabs(ballBorderY.y - ballBorderY.x + maxBallIndexY) / 3.0;
//      maxBallIndexU = fabs(ballBorderU.y - ballBorderU.x + maxBallIndexU) / 3.0;
//      maxBallIndexV = fabs(ballBorderV.y - ballBorderV.x + maxBallIndexV) / 3.0;
//      bPercept.ballImageMean.y = bPercept.meanImg.y;
//      bPercept.ballImageMean.u = bPercept.meanImg.u;
//      bPercept.ballImageMean.v = bPercept.meanImg.v;
//    }
//  );





////  if
////  (
////    bPercept.fieldIndex.y <= bPercept.meanEnv.y
////    &&
////    bPercept.fieldIndex.y <= bPercept.meanImg.y
//////    fabs(fPercept.maxWeightedIndexY - bPercept.fieldCalibIndex.y) < bPercept.fieldCalibDist.y / 2
//////    &&
//////    fabs(fPercept.maxWeightedIndexU - bPercept.fieldCalibIndex.u) < bPercept.fieldCalibDist.u / 2
//////    &&
//////    fabs(fPercept.maxWeightedIndexV - bPercept.fieldCalibIndex.v) < bPercept.fieldCalibDist.v / 2
////  )
////  {
////    bPercept.fieldIndex.y = (1 - adaptationRate) *  bPercept.fieldIndex.y + adaptationRate * fPercept.maxWeightedIndexY;
////    bPercept.fieldIndex.u =  (1 - adaptationRate) *  bPercept.fieldIndex.u + adaptationRate * fPercept.maxWeightedIndexU;
////    bPercept.fieldIndex.v =  (1 - adaptationRate) *  bPercept.fieldIndex.v + adaptationRate * fPercept.maxWeightedIndexV;
////  }

//  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_field",
//    bPercept.fieldIndex.y = fPercept.maxWeightedIndexY;
//    bPercept.fieldIndex.u = fPercept.maxWeightedIndexCb;
//    bPercept.fieldIndex.v = fPercept.maxWeightedIndexCr;
//    bPercept.fieldDist.y = fPercept.distY * 1.15;
//    bPercept.fieldDist.u = fPercept.distCb * 1.15;
//    bPercept.fieldDist.v = fPercept.distCr * 1.15;

//    bPercept.fieldImageMean.y = bPercept.meanImg.y;
//    bPercept.fieldImageMean.u = bPercept.meanImg.u;
//    bPercept.fieldImageMean.v = bPercept.meanImg.v;
//  );

//  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_lines",
//    bPercept.linesImageMean.y = bPercept.meanImg.y;
//    bPercept.linesImageMean.u = bPercept.meanImg.u;
//    bPercept.linesImageMean.v = bPercept.meanImg.v;
//  );

//  if
//  (
//    fabs(maxBallIndexY - BALL_IDX_Y) < bPercept.diff.y / 2
//    &&
//    fabs(maxBallIndexY - bPercept.ballIndex.y) < bPercept.diff.y / 2
//  )
//  {
//    bPercept.ballIndex.y =
//      (1 - adaptationRate) * bPercept.ballIndex.y + adaptationRate * maxBallIndexY;
//  }
//  if
//  (
//    fabs(maxBallIndexU - BALL_IDX_CB) < bPercept.diff.u / 2
//    &&
//    fabs(maxBallIndexU - bPercept.ballIndex.u) < bPercept.diff.u / 2
//  )
//  {
//    bPercept.ballIndex.u =
//      (1 - adaptationRate) * bPercept.ballIndex.u + adaptationRate * maxBallIndexU;
//  }
//  if
//  (
//    fabs(maxBallIndexV - BALL_IDX_CR) < bPercept.diff.v / 2
//    &&
//    fabs(maxBallIndexV - bPercept.ballIndex.v) < bPercept.diff.v / 2
//  )
//  {
//    bPercept.ballIndex.v =
//      (1 - adaptationRate) * bPercept.ballIndex.v + adaptationRate * maxBallIndexV;
//  }

//  if(reliableGoalPostCount >= 1)
//  {
//    maxGoalIndexY = (goalBorderY.y + goalBorderY.x) / 2.0;//((goalBorderY.y + goalBorderY.x) / 2.0 + maxGoalIndexY) / 2.0;
////    maxGoalIndexU = (goalBorderU.y + goalBorderU.x + maxGoalIndexU) / 3.0;
////    maxGoalIndexV = (goalBorderV.y + goalBorderV.x + maxGoalIndexV) / 3.0;

////    LINE_PX
////    (
////      ColorClasses::gray,
////      Math::clamp((int) (goalBorderY.x), 0, 255),
////      0,
////      Math::clamp((int) (goalBorderY.x), 0, 255),
////      120
////    );
////    LINE_PX
////    (
////      ColorClasses::gray,
////      Math::clamp((int) (goalBorderY.y), 0, 255),
////      0,
////      Math::clamp((int) (goalBorderY.y), 0, 255),
////      120
////    );
////    LINE_PX
////    (
////      ColorClasses::white,
////      Math::clamp((int) (goalBorderY.y + goalBorderY.x)/2, 0, 255),
////      0,
////      Math::clamp((int) (goalBorderY.y + goalBorderY.x)/2, 0, 255),
////      120
////    );

////    LINE_PX
////    (
////      ColorClasses::blue,
////      Math::clamp((int) (goalBorderU.x), 0, 255),
////      0,
////      Math::clamp((int) (goalBorderU.x), 0, 255),
////      120
////    );
////    LINE_PX
////    (
////      ColorClasses::blue,
////      Math::clamp((int) (goalBorderU.y), 0, 255),
////      0,
////      Math::clamp((int) (goalBorderU.y), 0, 255),
////      120
////    );
////    LINE_PX
////    (
////      ColorClasses::skyblue,
////      Math::clamp((int) (goalBorderU.y + goalBorderU.x)/2, 0, 255),
////      0,
////      Math::clamp((int) (goalBorderU.y + goalBorderU.x)/2, 0, 255),
////      120
////    );
////    LINE_PX
////    (
////      ColorClasses::orange,
////      Math::clamp((int) (maxGoalIndexV), 0, 255),
////      0,
////      Math::clamp((int) (maxGoalIndexV), 0, 255),
////      120
////    );

////    if(fabs(maxGoalIndexY - bPercept.goalCalibIndex.y) < bPercept.goalCalibDist.y)
////    {
////      bPercept.goalIndex.y =
////        (1 - adaptationRate) * bPercept.goalIndex.y + adaptationRate * maxGoalIndexY;
////    }
////    if(fabs(maxGoalIndexU - bPercept.goalCalibIndex.u) < bPercept.goalCalibDist.u)
////    {
////      bPercept.goalIndex.u =
////        (1 - adaptationRate) * bPercept.goalIndex.u + adaptationRate * maxGoalIndexU;
////    }
////    if(fabs(maxGoalIndexV - bPercept.goalCalibIndex.v) < bPercept.goalCalibDist.v)
////    {
////      bPercept.goalIndex.v =
////        (1 - adaptationRate) * bPercept.goalIndex.v + adaptationRate * maxGoalIndexV;
////    }
//  }

//  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_goal",
//    if(goalBorderY.abs2() > 0 && goalBorderU.abs2() > 0  && goalBorderV.abs2() > 0)
//    {
////      if(goalIsCalibrating)
////      {
////        goalMeanY.init();
////        goalMeanU.init();
////        goalMeanV.init();
////      }
////      maxGoalIndexY = fabs(goalBorderY.y + goalBorderY.x + maxGoalIndexY) / 3.0;
////      maxGoalIndexU = fabs(goalBorderU.y + goalBorderU.x + maxGoalIndexU) / 3.0;
////      maxGoalIndexV = fabs(goalBorderV.y + goalBorderV.x + maxGoalIndexV) / 3.0;

//      bPercept.goalIndex.y = maxGoalIndexY;
//      bPercept.goalIndex.u = maxGoalIndexU;
//      bPercept.goalIndex.v = maxGoalIndexV;

////      goalMeanY.add(bPercept.meanImg.y);
////      goalMeanU.add(bPercept.meanImg.u);
////      goalMeanV.add(bPercept.meanImg.v);

//      bPercept.goalImageMean.y = bPercept.meanImg.y;
//      bPercept.goalImageMean.u = bPercept.meanImg.u;
//      bPercept.goalImageMean.v = bPercept.meanImg.v;
////      bPercept.goalImageMean.y = goalMeanY.getAverage();//bPercept.meanEnv.y;
////      bPercept.goalImageMean.u = goalMeanU.getAverage();//bPercept.meanEnv.u;
////      bPercept.goalImageMean.v = goalMeanV.getAverage();//bPercept.meanEnv.v;
////      goalIsCalibrating = true;
////      LINE_PX
////      (
////        ColorClasses::gray,
////        Math::clamp((int) (maxGoalIndexY), 0, 255),
////        0,
////        Math::clamp((int) (maxGoalIndexY), 0, 255),
////        120
////      );
//      }
//    );

////  maxLineIndexY = LINE_IDX_Y * coloredGrid.meanBrightness / 128;
////  if(fabs(maxLineIndexY - LINE_IDX_Y) < bPercept.diff.y / 2)
////  {
////    bPercept.lineIndex.y =
////      (1 - adaptationRate) * bPercept.lineIndex.y + adaptationRate * maxLineIndexY;
////  }
////  maxLineIndexU = LINE_IDX_u * coloredGrid.meanBlue / 128;
////  if(fabs(maxLineIndexU - LINE_IDX_u) < bPercept.diff.u / 2)
////  {
////    bPercept.lineIndex.u =
////      (1 - adaptationRate) * bPercept.lineIndex.u + adaptationRate * maxLineIndexU;
////  }
////  maxLineIndexV = LINE_IDX_v * coloredGrid.meanRed / 128;
////  if(fabs(maxLineIndexV - LINE_IDX_v) < bPercept.diff.v / 2)
////  {
////    bPercept.lineIndex.v =
////      (1 - adaptationRate) * bPercept.lineIndex.v + adaptationRate * maxLineIndexV;
////  }

  bPercept.lastUpdated = getFrameInfo();

  bPercept.setPerceptRegions();

  runDebugRequests();



//  getBaseColorRegionPercept().VeatePercept();
}//end execute

void BaseColorClassifier::runDebugRequests()
{
  int imageWidth = getImage().cameraInfo.resolutionWidth;
  int imageHeight = getImage().cameraInfo.resolutionHeight;

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_ball_in_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(bPercept.isRedOrOrangeOrPink(pixel))
        {
          POINT_PX(ColorClasses::red, x, y);
        }

      }
    }
  );
  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_goal_in_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(bPercept.isYellow(pixel))
        {
          POINT_PX(ColorClasses::yellow, x, y);
        }

      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_lines_in_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(bPercept.isWhite(pixel))
        {
          POINT_PX(ColorClasses::black, x, y);
        }

      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_field_in_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(bPercept.isGreenOrBlue(pixel))
        {
          POINT_PX(ColorClasses::green, x, y);
        }

      }
    }
  );

  bool drawRegion = false;

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_y",
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) (bPercept.ballBorderMinus.y), 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderMinus.y), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) (bPercept.ballBorderPlus.y), 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderPlus.y), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::white,
      Math::clamp((int) (bPercept.ballBorderPlus.y + bPercept.ballBorderMinus.y) / 2, 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderPlus.y + bPercept.ballBorderMinus.y) / 2, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_u",
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (bPercept.ballBorderMinus.u), 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderMinus.u), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (bPercept.ballBorderPlus.u), 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderPlus.u), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::skyblue,
      Math::clamp((int) (bPercept.ballBorderPlus.u + bPercept.ballBorderMinus.u) / 2, 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderPlus.u + bPercept.ballBorderMinus.u) / 2, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_v",
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (bPercept.ballBorderMinus.v), 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderMinus.v), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (bPercept.ballBorderPlus.v), 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderPlus.v), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::orange,
      Math::clamp((int) (bPercept.ballBorderPlus.v + bPercept.ballBorderMinus.v) / 2, 0, 255),
      0,
      Math::clamp((int) (bPercept.ballBorderPlus.v + bPercept.ballBorderMinus.v) / 2, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_y",
      LINE_PX
      (
        ColorClasses::gray,
        Math::clamp((int) (bPercept.goalBorderMinus.y), 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderMinus.y), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::gray,
        Math::clamp((int) (bPercept.goalBorderPlus.y), 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderPlus.y), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::white,
        Math::clamp((int) (bPercept.goalBorderPlus.y + bPercept.goalBorderMinus.y) / 2, 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderPlus.y + bPercept.goalBorderMinus.y) / 2, 0, 255),
        imageHeight
      );
      drawRegion = true;
    );

    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_u",
      LINE_PX
      (
        ColorClasses::blue,
        Math::clamp((int) (bPercept.goalBorderMinus.u), 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderMinus.u), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::blue,
        Math::clamp((int) (bPercept.goalBorderPlus.u), 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderPlus.u), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::skyblue,
        Math::clamp((int) (bPercept.goalBorderPlus.u + bPercept.goalBorderMinus.u) / 2, 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderPlus.u + bPercept.goalBorderMinus.u) / 2, 0, 255),
        imageHeight
      );
      drawRegion = true;
    );

    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_v",
      LINE_PX
      (
        ColorClasses::red,
        Math::clamp((int) (bPercept.goalBorderMinus.v), 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderMinus.v), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::red,
        Math::clamp((int) (bPercept.goalBorderPlus.v), 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderPlus.v), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::orange,
        Math::clamp((int) (bPercept.goalBorderPlus.v + bPercept.goalBorderMinus.v) / 2, 0, 255),
        0,
        Math::clamp((int) (bPercept.goalBorderPlus.v + bPercept.goalBorderMinus.v) / 2, 0, 255),
        imageHeight
      );
      drawRegion = true;
    );

      DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_y",
        LINE_PX
        (
          ColorClasses::gray,
          Math::clamp((int) (bPercept.fieldBorderMinus.y), 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderMinus.y), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::gray,
          Math::clamp((int) (bPercept.fieldBorderPlus.y), 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderPlus.y), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::white,
          Math::clamp((int) (bPercept.fieldBorderPlus.y + bPercept.fieldBorderMinus.y) / 2, 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderPlus.y + bPercept.fieldBorderMinus.y) / 2, 0, 255),
          imageHeight
        );
        drawRegion = true;
      );

      DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_u",
        LINE_PX
        (
          ColorClasses::blue,
          Math::clamp((int) (bPercept.fieldBorderMinus.u), 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderMinus.u), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::blue,
          Math::clamp((int) (bPercept.fieldBorderPlus.u), 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderPlus.u), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::skyblue,
          Math::clamp((int) (bPercept.fieldBorderPlus.u + bPercept.fieldBorderMinus.u) / 2, 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderPlus.u + bPercept.fieldBorderMinus.u) / 2, 0, 255),
          imageHeight
        );
        drawRegion = true;
      );

      DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_v",
        LINE_PX
        (
          ColorClasses::red,
          Math::clamp((int) (bPercept.fieldBorderMinus.v), 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderMinus.v), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::red,
          Math::clamp((int) (bPercept.fieldBorderPlus.v), 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderPlus.v), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::orange,
          Math::clamp((int) (bPercept.fieldBorderPlus.v + bPercept.fieldBorderMinus.v) / 2, 0, 255),
          0,
          Math::clamp((int) (bPercept.fieldBorderPlus.v + bPercept.fieldBorderMinus.v) / 2, 0, 255),
          imageHeight
        );
        drawRegion = true;
      );


  if(drawRegion)
  {
    LINE_PX
    (
      ColorClasses::black,
      256,
      imageHeight,
      256,
      imageHeight
    );
   }

}
