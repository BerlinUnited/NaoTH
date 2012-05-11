#include "BaseColorClassifier.h"

BaseColorClassifier::BaseColorClassifier()
:
  cTable(getColorTable64()),
  histogram(getHistogram()),
  coloredGrid(getColoredGrid()),
  calibCount(0)
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_ball_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_lines_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_field_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_gray_in_image", " ", false);

  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_simple_goal_in_image", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_image_plain", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_corrected_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:initWithPerceptParameters", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_ball", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_field", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_lines", " ", false);

  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:field_y", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:field_u", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:field_v", " ", false);

  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_y", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_u", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_v", " ", false);

  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_y", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_u", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_v", " ", false);

  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_colors", " ", false);
  //DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:reset_calibration", " ", false);

  lastMeanY = coloredGrid.meanBrightness;
  lastMeanU = coloredGrid.meanBlue;
  lastMeanV = coloredGrid.meanRed;
  adaptationRate = 0.9;
  initPercepts();
  getBaseColorRegionPercept().setPerceptRegions();
  goalIsCalibrating = false;
}

void BaseColorClassifier::initPercepts()
{
  getBaseColorRegionPercept().distGray = regionParams.distGray;
  getBaseColorRegionPercept().grayLevel = regionParams.grayLevel;

  getBaseColorRegionPercept().fieldCalibIndex = regionParams.fieldIndex;
  getBaseColorRegionPercept().fieldIndex = regionParams.fieldIndex;
  getBaseColorRegionPercept().fieldCalibDist = regionParams.fieldDist;
  getBaseColorRegionPercept().fieldDist = regionParams.fieldDist;

  getBaseColorRegionPercept().goalVUdistance = regionParams.goalVUdistance;
  getBaseColorRegionPercept().goalVUdistanceMin = regionParams.goalVUdistanceMin;
  getBaseColorRegionPercept().goalVUdistanceMax = regionParams.goalVUdistanceMax;

  getBaseColorRegionPercept().goalCalibIndex = regionParams.goalIndex;
  getBaseColorRegionPercept().goalIndex = regionParams.goalIndex;
  getBaseColorRegionPercept().goalCalibDist = regionParams.goalDist;
  getBaseColorRegionPercept().goalDist = regionParams.goalDist;

  getBaseColorRegionPercept().ballCalibIndex = regionParams.ballIndex;
  getBaseColorRegionPercept().ballIndex = regionParams.ballIndex;
  getBaseColorRegionPercept().ballCalibDist = regionParams.ballDist;
  getBaseColorRegionPercept().ballDist = regionParams.ballDist;

  getBaseColorRegionPercept().lineCalibIndex = regionParams.lineIndex;
  getBaseColorRegionPercept().lineIndex = regionParams.lineIndex;
  getBaseColorRegionPercept().lineCalibDist = regionParams.lineDist;
  getBaseColorRegionPercept().lineDist = regionParams.lineDist;
}


void BaseColorClassifier::execute()
{
  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:initWithPerceptParameters",
    initPercepts();
  );

  //DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:reset_calibration",
  //  calibCount = 0;
  //);

  //DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_colors",
  //  if(calibCount == 0)
  //  {
  //    getBaseColorRegionPercept().distGray = regionParams.distGray;
  //    getBaseColorRegionPercept().grayLevel = regionParams.grayLevel;

  //    getBaseColorRegionPercept().goalVUdistance = regionParams.goalVUdistance;
  //    getBaseColorRegionPercept().goalVUdistanceMin = regionParams.goalVUdistanceMin;
  //    getBaseColorRegionPercept().goalVUdistanceMax = regionParams.goalVUdistanceMax;

  //    getBaseColorRegionPercept().setPerceptRegions();

  //    maxWeighted.y = 0.0;
  //    maxWeighted.u = 0.0;
  //    maxWeighted.v = 0.0;
  //    maxWeightedIndex.y = -1;
  //    maxWeightedIndex.u = -1;
  //    maxWeightedIndex.v = -1;

  //    memset(&fieldWeightedHist, 0, sizeof(fieldWeightedHist));  
  //  
  //    fieldCalibMeanY = 0.0;
  //    fieldCalibMeanCountY = 1.0;
  //    memset(&fieldCalibHist, 0, sizeof(fieldCalibHist));

  //    minDistVminusU = regionParams.goalVUdistanceMin;
  //    maxDistVminusU = regionParams.goalVUdistanceMax;
  //    sampleMinDistVu.add(Math::clamp<int>(minDistVminusU - 5, 0, 255));
  //    sampleMaxDistVu.add(Math::clamp<int>(maxDistVminusU + 5, 0, 255));

  //    minEnv.y = 255;
  //    minEnv.u = 255;
  //    minEnv.v = 255;
  //    maxEnv.y = 0;
  //    maxEnv.u = 0;
  //    maxEnv.v = 0;
  //
  //    minYellow.y = 255;
  //    minYellow.u = 255;
  //    minYellow.v = 255;
  //    maxYellow.y = 0;
  //    maxYellow.u = 0;
  //    maxYellow.v = 0;
  //  }
  //  
  //  if(calibCount < 200)
  //  {
  //    calibrateColorRegions();
  //    //calibCount++;
  //  }

  //  unsigned int gridPointNum = getColoredGrid().uniformGrid.numberOfGridPoints;
  //  for(unsigned int i = gridPointNum / 2; i < gridPointNum; i++)
  //  {
  //    Pixel pixel;
  //    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
  //    getImage().getCorrected(point.x, point.y, pixel);
  //    if
  //    (
  //      abs((int) pixel.y - (int) maxWeightedIndex.y) <= 120
  //      &&
  //      abs((int) pixel.u - (int) maxWeightedIndex.u) <= distField.u
  //      &&
  //      abs((int) pixel.v - (int) maxWeightedIndex.v) <= distField.v
  //    )
  //    {
  //      POINT_PX(ColorClasses::blue, point.x, point.y);
  //    }
  //    else 
  //    {
  //      int distanceVminusU = pixel.v - pixel.u;
  //      if
  //      (
  //        distanceVminusU > minDistVminusU
  //        &&
  //        distanceVminusU < maxDistVminusU
  //        //&&
  //        //isInRegion(minYellow, maxYellow, pixel)
  //      )
  //      {
  //        POINT_PX(ColorClasses::white, point.x, point.y);
  //      }
  //    }
  //  }

  //  for(unsigned int x = 0; x < getImage().width(); x++)
  //  {
  //    for(unsigned int y = 0; y < getImage().height(); y++)
  //    {
  //      const Pixel& pixel = getImage().get(x, y);
  //      int distanceVminusU = pixel.v - pixel.u;
  //      if
  //      (
  //        getBaseColorRegionPercept().isPlainYellow(pixel)
  //        &&
  //        distanceVminusU > minDistVminusU
  //        &&
  //        distanceVminusU < maxDistVminusU
  //        //&&
  //        //isInRegion(minYellow, maxYellow, pixel)
  //      )
  //      {
  //        POINT_PX(ColorClasses::yellowOrange, x, y);
  //      }

  //    }
  //  }
  //);

  setColorRegions();

  runDebugRequests();
}//end execute

void BaseColorClassifier::calibrateColorRegions()
{
  calibrateFieldColorRegion();

}

void BaseColorClassifier::calibrateFieldColorRegion()
{
  weighted.y = 0.0;
  weighted.u = 0.0;
  weighted.v = 0.0;

  unsigned int gridPointNum = getColoredGrid().uniformGrid.numberOfGridPoints;

  memset(&fieldHist, 0, sizeof(fieldHist));

  for(unsigned int i = gridPointNum / 2; i < gridPointNum; i++)
  {
    Pixel pixel;
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
    getImage().getCorrected(point.x, point.y, pixel);

    fieldHist[0][pixel.y]++;
    fieldHist[1][pixel.u]++;
    fieldHist[2][pixel.v]++;
  }

  static const int halfChannelWidth = COLOR_CHANNEL_VALUE_COUNT / 2;
  static const double histogramStep = 1.0/(double) COLOR_CHANNEL_VALUE_COUNT;
  static const double histogramdoubleStep = 2.0*histogramStep;

  for(int i = 0; i < halfChannelWidth; i++)
  {
    // apply the weght max(0,128-i)/128 = 1-i/128 for i <= 128
    double wCr = 1.0 - i * histogramdoubleStep;
    weighted.v = wCr * (double) fieldHist[2][i];
    fieldWeightedHist[2][i] = weighted.v;
    PLOT_GENERIC("BaseColorClassifier:weighted.v", i, weighted.v);

    // search for max Cr channel value with weight w
    if(weighted.v > maxWeighted.v)
    {
      //maxWeighted.v = maxWeighted.v / 2 + weighted.v / 2;
      //maxWeightedIndex.v = maxWeightedIndex.v / 2 + i / 2;
      maxWeighted.v = weighted.v;
      maxWeightedIndex.v = i;
    }
  }//end for


  for(unsigned int i = gridPointNum / 2; i < gridPointNum; i++)
  {
    Pixel pixel;
    const Vector2<int>& point = getColoredGrid().uniformGrid.getPoint(i);
    getImage().getCorrected(point.x, point.y, pixel);

    fieldCalibHist[0][pixel.y]++;
    fieldCalibMeanY += pixel.y;
    fieldCalibMeanCountY++;
    fieldCalibHist[1][pixel.u]++;
  }

  double meanFieldY = 0;
  double numberOfFieldY = 0;

  for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    // weight based on the mean value  (255 - i)/255
    double wCb = 1.0 - i * histogramStep;
    weighted.u = wCb * fieldCalibHist[1][i];
    fieldWeightedHist[1][i] = weighted.u;
    PLOT_GENERIC("BaseColorClassifier:weighted.u", i, weighted.u);

    // calculate the Cb maximum
    if(weighted.u > maxWeighted.u)
    {
      //maxWeighted.u = maxWeighted.u / 2 + weighted.u / 2;
      //maxWeightedIndex.u = maxWeightedIndex.u / 2 + i / 2;
      maxWeighted.u = weighted.u;
      maxWeightedIndex.u = i;
    }

    PLOT_GENERIC("BaseColorClassifier:weighted.y", i, fieldCalibHist[0][i]);
    fieldWeightedHist[0][i] = fieldCalibHist[0][i];

    meanFieldY += fieldCalibHist[0][i] * i;
    numberOfFieldY += fieldCalibHist[0][i];
  }//end for


  if(numberOfFieldY > 0)
  {
    meanFieldY /= numberOfFieldY;
  }

  //check how it works in other conditions
  maxWeightedIndex.y = maxWeightedIndex.y / 2  + (int) Math::clamp(meanFieldY,0.0, 255.0) / 2;

  unsigned int meanRegionBeginIndexCb = 0;
  unsigned int meanRegionEndIndexCb = 0;

  // find the left Cr-limit
  // (move from maxWeightedIndexCr to 0)
  unsigned int idx = maxWeightedIndex.u;
  while (idx >= 0 && meanRegionBeginIndexCb == 0)
  {
    if(fieldWeightedHist[2][idx] <= 0.5)//<= log(maxWeightedCr) || maxWeightedIndexCr - idx > maxDistCr)
    {
      meanRegionBeginIndexCb = idx;
    }
    idx--;
  }

  // find the right Cb-limit
  // (move from maxWeightedIndexCr to COLOR_CHANNEL_VALUE_COUNT)
  idx = maxWeightedIndex.v;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCb == 0)
  {
    if(fieldWeightedHist[1][idx] <= 0.5)//<= log(maxWeightedCr) || idx - maxWeightedIndexCr > maxDistCr)
    {
      meanRegionEndIndexCb = idx;
    }
    idx++;
  }

  unsigned int meanRegionBeginIndexCr = 0;
  unsigned int meanRegionEndIndexCr = 0;

  // find the left Cr-limit
  // (move from maxWeightedIndexCr to 0)
  idx = maxWeightedIndex.v;
  while (idx >= 0 && meanRegionBeginIndexCr == 0)
  {
    if(fieldWeightedHist[2][idx] <= 0.5)//<= log(maxWeightedCr) || maxWeightedIndexCr - idx > maxDistCr)
    {
      meanRegionBeginIndexCr = idx;
    }
    idx--;
  }

  // find the right Cr-limit
  // (move from maxWeightedIndexCr to COLOR_CHANNEL_VALUE_COUNT)
  idx = maxWeightedIndex.v;
  while (idx < COLOR_CHANNEL_VALUE_COUNT && meanRegionEndIndexCr == 0)
  {
    if(fieldWeightedHist[2][idx] <= 0.5)//<= log(maxWeightedCr) || idx - maxWeightedIndexCr > maxDistCr)
    {
      meanRegionEndIndexCr = idx;
    }
    idx++;
  }
  
  distField.u = distField.u / 2 + abs((int) meanRegionBeginIndexCb - (int) meanRegionEndIndexCb) / 4;
  distField.v = distField.v / 2 + abs((int) meanRegionBeginIndexCr - (int) meanRegionEndIndexCr) / 4;

  getBaseColorRegionPercept().goalBorderMinus.y = Math::clamp<int>(maxWeightedIndex.y, 0, 255);
  getBaseColorRegionPercept().goalColorRegion.set(getBaseColorRegionPercept().goalBorderMinus, getBaseColorRegionPercept().goalBorderPlus);

  bool greenBorderfound = false;

  unsigned int y = getImage().height();

  Pixel testPixel;
  Pixel left;
  Pixel right;

  if(false)
  {
    for(unsigned int x = 1; x < getImage().width() - 2; x++)
    {
      getImage().getCorrected(x, 220, testPixel);
      POINT_PX(ColorClasses::pink, x,220);

      if(minEnv.y >  testPixel.y) minEnv.y = minEnv.y / 2 + testPixel.y / 2;
      if(minEnv.u >  testPixel.u) minEnv.u = minEnv.u / 2 + testPixel.u / 2;
      if(minEnv.v >  testPixel.v) minEnv.v = minEnv.v / 2 + testPixel.v / 2;

      if(maxEnv.y <  testPixel.y) maxEnv.y = maxEnv.y / 2 + testPixel.y / 2;
      if(maxEnv.u <  testPixel.u) maxEnv.u = maxEnv.u / 2 + testPixel.u / 2;
      if(maxEnv.v <  testPixel.v) maxEnv.v = maxEnv.v / 2 + testPixel.v / 2;

      getImage().getCorrected(x, 200, testPixel);
      POINT_PX(ColorClasses::pink, x, 200);

      if(minEnv.y >  testPixel.y) minEnv.y = minEnv.y / 2 + testPixel.y / 2;
      if(minEnv.u >  testPixel.u) minEnv.u = minEnv.u / 2 + testPixel.u / 2;
      if(minEnv.v >  testPixel.v) minEnv.v = minEnv.v / 2 + testPixel.v / 2;

      if(maxEnv.y <  testPixel.y) maxEnv.y = maxEnv.y / 2 + testPixel.y / 2;
      if(maxEnv.u <  testPixel.u) maxEnv.u = maxEnv.u / 2 + testPixel.u / 2;
      if(maxEnv.v <  testPixel.v) maxEnv.v = maxEnv.v / 2 + testPixel.v / 2;
    }

    for(unsigned int x = 1; x < getImage().width() - 2; x++)
    {

      for(unsigned int yy = 120; yy >= 20; yy -= 20)
      {
        getImage().getCorrected(x, y - yy, testPixel);
        POINT_PX(ColorClasses::pink, x, y - yy);

        if(!isInRegion(minEnv, maxEnv, testPixel))
        {
          POINT_PX(ColorClasses::red, x, y - yy);
          int distanceVminusU = testPixel.v - testPixel.u;

          if(distanceVminusU > maxDistVminusU) maxDistVminusU = maxDistVminusU / 2 + distanceVminusU / 2;
          if(distanceVminusU < minDistVminusU) minDistVminusU = minDistVminusU / 2 + distanceVminusU / 2;
        }
      }
      //getImage().getCorrected(x, y - 140, testPixel);
      //POINT_PX(ColorClasses::pink, x, y - 140);

      //if(!isInRegion(minEnv, maxEnv, testPixel))
      //{
      //  POINT_PX(ColorClasses::red, x, y - 140);
      //  int distanceVminusU = testPixel.v - testPixel.u;

      //  if(distanceVminusU > maxDistVminusU) maxDistVminusU = maxDistVminusU / 2 + distanceVminusU / 2;
      //  if(distanceVminusU < minDistVminusU) minDistVminusU = minDistVminusU / 2 + distanceVminusU / 2;
      //}
    }
  }

  if(true)
  {
    y = getImage().height() / 2;

    while(!greenBorderfound && y < getImage().height() - 11)
    {
      y++;
      POINT_PX(ColorClasses::skyblue, 1, y);
      POINT_PX(ColorClasses::skyblue, getImage().width() - 2, y);
      getImage().getCorrected(1, y, left);
      getImage().getCorrected(getImage().width() - 2, y, right);

      greenBorderfound = abs((int) left.y - (int) maxWeightedIndex.y) <= 120
                          &&
                          abs((int) left.u - (int) maxWeightedIndex.u) <= distField.u
                          &&
                          abs((int) left.v - (int) maxWeightedIndex.v) <= distField.v
                          &&
                          abs((int) right.y - (int) maxWeightedIndex.y) <= 120
                          &&
                          abs((int) right.u - (int) maxWeightedIndex.u) <= distField.u
                          &&
                          abs((int) right.v - (int) maxWeightedIndex.v) <= distField.v;
    }

    y += 1;

    unsigned int noGreenEndIdx = getImage().width() - 2;
    unsigned int x = 1;
    bool greenEnded = false;

    int minDistVU = 255;
    int maxDistVU = 0;

    while(x < noGreenEndIdx)
    {
      getImage().getCorrected(x, y, testPixel);
      POINT_PX(ColorClasses::pink, x, y);

      if
      (
        (
          abs((int) testPixel.y - (int) maxWeightedIndex.y) > 120
          ||
          abs((int) testPixel.u - (int) maxWeightedIndex.u) > distField.u
          ||
          abs((int) testPixel.v - (int) maxWeightedIndex.v) > distField.v
        )
      )    
      {
        if(!greenEnded)
        {
          greenEnded = true;
          noGreenEndIdx = x + 20;
          if(noGreenEndIdx > getImage().width() - 2)
          {
            noGreenEndIdx = getImage().width() - 2;
          }
        }
        else if(getBaseColorRegionPercept().isPlainYellow(testPixel) && !getBaseColorRegionPercept().isWhite(testPixel))
        {
          POINT_PX(ColorClasses::red, x, y);
          int distanceVminusU = testPixel.v - testPixel.u;

          //if(distanceVminusU > maxDistVminusU) maxDistVminusU = maxDistVminusU / 2 + distanceVminusU / 2;
          //if(distanceVminusU < minDistVminusU) minDistVminusU = minDistVminusU / 2 + distanceVminusU / 2;
          if(distanceVminusU > maxDistVU) maxDistVU = distanceVminusU;
          if(distanceVminusU < minDistVU) minDistVU = distanceVminusU;

          //if(minYellow.y >  testPixel.y) minYellow.y = minYellow.y / 2 + testPixel.y / 2;
          //if(minYellow.u >  testPixel.u) minYellow.u = minYellow.u / 2 + testPixel.u / 2;
          //if(minYellow.v >  testPixel.v) minYellow.v = minYellow.v / 2 + testPixel.v / 2;

          //if(maxYellow.y <  testPixel.y) maxYellow.y = maxYellow.y / 2 + testPixel.y / 2;
          //if(maxYellow.u <  testPixel.u) maxYellow.u = maxYellow.u / 2 + testPixel.u / 2;
          //if(maxYellow.v <  testPixel.v) maxYellow.v = maxYellow.v / 2 + testPixel.v / 2;
        }
      }
      else if(greenEnded)
      {
        x = noGreenEndIdx;
        greenEnded = false;
      }

      x++;
    }

    if(minDistVU < maxDistVU)
    {
      sampleMinDistVu.add(Math::clamp<int>(minDistVU - 5, 0, 255));
      sampleMaxDistVu.add(Math::clamp<int>(maxDistVU + 5, 0, 255));
      minDistVminusU = sampleMinDistVu.getAverage();
      maxDistVminusU = sampleMaxDistVu.getAverage();
      calibCount++;
    }

    minDistVU = 255;
    maxDistVU = 0;

    x = getImage().width() - 2;
    noGreenEndIdx = 1;
    greenEnded = false;

    while(x > noGreenEndIdx)
    {
      getImage().getCorrected(x, y, testPixel);
      POINT_PX(ColorClasses::pink, x, y);

      if
      (
        (
          abs((int) testPixel.y - (int) maxWeightedIndex.y) > 120
          ||
          abs((int) testPixel.u - (int) maxWeightedIndex.u) > distField.u
          ||
          abs((int) testPixel.v - (int) maxWeightedIndex.v) > distField.v
        )
      )    
      {
        if(!greenEnded)
        {
          greenEnded = true;
          noGreenEndIdx = x - 20;
          if(noGreenEndIdx < 1)
          {
            noGreenEndIdx = 1;
          }
        }
        else if(getBaseColorRegionPercept().isPlainYellow(testPixel) && !getBaseColorRegionPercept().isWhite(testPixel))
        {
          POINT_PX(ColorClasses::red, x, y);
          int distanceVminusU = testPixel.v - testPixel.u;

          //if(distanceVminusU > maxDistVminusU) maxDistVminusU = maxDistVminusU / 2 + distanceVminusU / 2;
          //if(distanceVminusU < minDistVminusU) minDistVminusU = minDistVminusU / 2 + distanceVminusU / 2;
          if(distanceVminusU > maxDistVU) maxDistVU = distanceVminusU;
          if(distanceVminusU < minDistVU) minDistVU = distanceVminusU;

          //if(minYellow.y >  testPixel.y) minYellow.y = minYellow.y / 2 + testPixel.y / 2;
          //if(minYellow.u >  testPixel.u) minYellow.u = minYellow.u / 2 + testPixel.u / 2;
          //if(minYellow.v >  testPixel.v) minYellow.v = minYellow.v / 2 + testPixel.v / 2;

          //if(maxYellow.y <  testPixel.y) maxYellow.y = maxYellow.y / 2 + testPixel.y / 2;
          //if(maxYellow.u <  testPixel.u) maxYellow.u = maxYellow.u / 2 + testPixel.u / 2;
          //if(maxYellow.v <  testPixel.v) maxYellow.v = maxYellow.v / 2 + testPixel.v / 2;
        }
      }
      else if(greenEnded)
      {
        x = noGreenEndIdx;
        greenEnded = false;
      }

      x--;
    }

    if(minDistVU < maxDistVU)
    {
      sampleMinDistVu.add(Math::clamp<int>(minDistVU - 5, 0, 255));
      sampleMaxDistVu.add(Math::clamp<int>(maxDistVU + 5, 0, 255));
      minDistVminusU = sampleMinDistVu.getAverage();
      maxDistVminusU = sampleMaxDistVu.getAverage();
      calibCount++;
    }


    getBaseColorRegionPercept().goalVUdistanceMin = minDistVminusU;
    getBaseColorRegionPercept().goalVUdistanceMax = maxDistVminusU;
    getBaseColorRegionPercept().setPerceptRegions();
  }
}

bool BaseColorClassifier::isInRegion(Pixel min, Pixel max, Pixel toTest)
{
  return  min.y < toTest.y
          &&
          toTest.y < max.y
          &&
          min.u < toTest.u
          &&
          toTest.u < max.u
          &&
          min.v < toTest.v
          &&
          toTest.v < max.v;
}


void BaseColorClassifier::setColorRegions()
{
  getBaseColorRegionPercept().meanImg.y = coloredGrid.meanBrightness;
  getBaseColorRegionPercept().meanImg.u = coloredGrid.meanBlue;
  getBaseColorRegionPercept().meanImg.v = coloredGrid.meanRed;

  double diff = fabs(coloredGrid.meanBrightness - lastMeanY);
  if(diff > 5.0)
  {
    meanY.add(coloredGrid.meanBrightness);
    getBaseColorRegionPercept().meanEnv.y = meanY.getAverage();
    getBaseColorRegionPercept().diff.y = diff;
    lastMeanY = coloredGrid.meanBrightness;
  }
  diff = fabs(coloredGrid.meanBlue - lastMeanU);
  if(diff > 5.0)
  {
    meanU.add(coloredGrid.meanBlue);
    getBaseColorRegionPercept().meanEnv.u = meanU.getAverage();
    getBaseColorRegionPercept().diff.u = diff;
    lastMeanU = coloredGrid.meanBlue;
  }
  diff = fabs(coloredGrid.meanRed - lastMeanV);
  if(diff > 5.0)
  {
    meanV.add(coloredGrid.meanRed);
    getBaseColorRegionPercept().meanEnv.v = meanV.getAverage();
    getBaseColorRegionPercept().diff.v = diff;
    lastMeanV = coloredGrid.meanRed;
  }
  getBaseColorRegionPercept().distGray = regionParams.distGray;
  getBaseColorRegionPercept().grayLevel = regionParams.grayLevel;

  getBaseColorRegionPercept().goalVUdistance = regionParams.goalVUdistance;
  getBaseColorRegionPercept().goalVUdistanceMin = regionParams.goalVUdistanceMin;
  getBaseColorRegionPercept().goalVUdistanceMax = regionParams.goalVUdistanceMax;

  getBaseColorRegionPercept().setPerceptRegions();

  //getBaseColorRegionPercept().lineBorderMinus.y = Math::clamp<double>(getFieldColorPercept().maxWeightedIndex.y + getFieldColorPercept().dist.y, 0.0, 255.0);
  //getBaseColorRegionPercept().lineBorderPlus.y = 255.0;

  //getBaseColorRegionPercept().lineColorRegion.set(getBaseColorRegionPercept().lineBorderMinus, getBaseColorRegionPercept().lineBorderPlus);
  //
  //getBaseColorRegionPercept().goalBorderMinus.y = Math::clamp<double>(getFieldColorPercept().maxWeightedIndex.y/* + getFieldColorPercept().dist.y*/, 0.0, 255.0);

  //getBaseColorRegionPercept().goalColorRegion.set(getBaseColorRegionPercept().goalBorderMinus, getBaseColorRegionPercept().goalBorderPlus);

  //getBaseColorRegionPercept().ballBorderMinus.v = Math::clamp<double>(getBaseColorRegionPercept().goalBorderPlus.v, 0.0, 255.0);

  //getBaseColorRegionPercept().ballColorRegion.set(getBaseColorRegionPercept().ballBorderMinus, getBaseColorRegionPercept().ballBorderPlus);

  getBaseColorRegionPercept().lastUpdated = getFrameInfo();
}


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

        if(getBaseColorRegionPercept().isRedOrOrangeOrPink(pixel))
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

        if(getBaseColorRegionPercept().isYellow(pixel))
        {
          POINT_PX(ColorClasses::yellow, x, y);
        }

      }
    }
  );

  //DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_simple_goal_in_image",
  //  for(int x = 0; x < imageWidth; x++)
  //  {
  //    for(int y = 0; y < imageHeight; y++)
  //    {
  //      const Pixel& pixel = getImage().get(x, y);

  //      if(getBaseColorRegionPercept().isYellowSimple(pixel))
  //      {
  //        POINT_PX(ColorClasses::yellowOrange, x, y);
  //      }

  //    }
  //  }
  //);

  //DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_goal_in_corrected_image",
  //  for(int x = 0; x < imageWidth; x++)
  //  {
  //    for(int y = 0; y < imageHeight; y++)
  //    {
  //      const Pixel& pixel = getImage().getCorrected(x, y);

  //      if(getBaseColorRegionPercept().isYellow(pixel))
  //      {
  //        POINT_PX(ColorClasses::yellowOrange, x, y);
  //      }

  //    }
  //  }
  //);

  //DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_goal_in_image_plain",
  //  for(int x = 0; x < imageWidth; x++)
  //  {
  //    for(int y = 0; y < imageHeight; y++)
  //    {
  //      const Pixel& pixel = getImage().get(x, y);

  //      if(getBaseColorRegionPercept().isPlainYellow(pixel))
  //      {
  //        POINT_PX(ColorClasses::yellowOrange, x, y);
  //      }

  //    }
  //  }
  //);

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_lines_in_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isWhite(pixel))
        {
          POINT_PX(ColorClasses::black, x, y);
        }

      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_gray_in_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isGrayLevel(pixel))
        {
          POINT_PX(ColorClasses::gray, x, y);
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

        if(!getBaseColorRegionPercept().isGrayLevel(pixel) && getFieldColorPercept().isFieldColor(pixel))
        {
          POINT_PX(ColorClasses::green, x, y);
        }

      }
    }
  );

  //bool drawRegion = false;

  //DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_y",
  //  LINE_PX
  //  (
  //    ColorClasses::gray,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.y), 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.y), 0, 255),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::gray,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.y), 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.y), 0, 255),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::white,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.y + getBaseColorRegionPercept().ballBorderMinus.y) / 2, 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.y + getBaseColorRegionPercept().ballBorderMinus.y) / 2, 0, 255),
  //    imageHeight
  //  );
  //  drawRegion = true;
  //);

  //DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_u",
  //  LINE_PX
  //  (
  //    ColorClasses::blue,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.u), 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.u), 0, 255),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::blue,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.u), 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.u), 0, 255),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::skyblue,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.u + getBaseColorRegionPercept().ballBorderMinus.u) / 2, 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.u + getBaseColorRegionPercept().ballBorderMinus.u) / 2, 0, 255),
  //    imageHeight
  //  );
  //  drawRegion = true;
  //);

  //DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_v",
  //  LINE_PX
  //  (
  //    ColorClasses::red,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.v), 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.v), 0, 255),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::red,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.v), 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.v), 0, 255),
  //    imageHeight
  //  );
  //  LINE_PX
  //  (
  //    ColorClasses::orange,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.v + getBaseColorRegionPercept().ballBorderMinus.v) / 2, 0, 255),
  //    0,
  //    Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.v + getBaseColorRegionPercept().ballBorderMinus.v) / 2, 0, 255),
  //    imageHeight
  //  );
  //  drawRegion = true;
  //);

  //  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_y",
  //    LINE_PX
  //    (
  //      ColorClasses::gray,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.y), 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.y), 0, 255),
  //      imageHeight
  //    );
  //    LINE_PX
  //    (
  //      ColorClasses::gray,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.y), 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.y), 0, 255),
  //      imageHeight
  //    );
  //    LINE_PX
  //    (
  //      ColorClasses::white,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.y + getBaseColorRegionPercept().goalBorderMinus.y) / 2, 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.y + getBaseColorRegionPercept().goalBorderMinus.y) / 2, 0, 255),
  //      imageHeight
  //    );
  //    drawRegion = true;
  //  );

  //  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_u",
  //    LINE_PX
  //    (
  //      ColorClasses::blue,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.u), 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.u), 0, 255),
  //      imageHeight
  //    );
  //    LINE_PX
  //    (
  //      ColorClasses::blue,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.u), 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.u), 0, 255),
  //      imageHeight
  //    );
  //    LINE_PX
  //    (
  //      ColorClasses::skyblue,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.u + getBaseColorRegionPercept().goalBorderMinus.u) / 2, 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.u + getBaseColorRegionPercept().goalBorderMinus.u) / 2, 0, 255),
  //      imageHeight
  //    );
  //    drawRegion = true;
  //  );

  //  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_v",
  //    LINE_PX
  //    (
  //      ColorClasses::red,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.v), 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.v), 0, 255),
  //      imageHeight
  //    );
  //    LINE_PX
  //    (
  //      ColorClasses::red,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.v), 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.v), 0, 255),
  //      imageHeight
  //    );
  //    LINE_PX
  //    (
  //      ColorClasses::orange,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.v + getBaseColorRegionPercept().goalBorderMinus.v) / 2, 0, 255),
  //      0,
  //      Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.v + getBaseColorRegionPercept().goalBorderMinus.v) / 2, 0, 255),
  //      imageHeight
  //    );
  //    drawRegion = true;
  //  );

  //    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_y",
  //      LINE_PX
  //      (
  //        ColorClasses::gray,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.y), 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.y), 0, 255),
  //        imageHeight
  //      );
  //      LINE_PX
  //      (
  //        ColorClasses::gray,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.y), 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.y), 0, 255),
  //        imageHeight
  //      );
  //      LINE_PX
  //      (
  //        ColorClasses::white,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.y + getBaseColorRegionPercept().fieldBorderMinus.y) / 2, 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.y + getBaseColorRegionPercept().fieldBorderMinus.y) / 2, 0, 255),
  //        imageHeight
  //      );
  //      drawRegion = true;
  //    );

  //    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_u",
  //      LINE_PX
  //      (
  //        ColorClasses::blue,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.u), 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.u), 0, 255),
  //        imageHeight
  //      );
  //      LINE_PX
  //      (
  //        ColorClasses::blue,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.u), 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.u), 0, 255),
  //        imageHeight
  //      );
  //      LINE_PX
  //      (
  //        ColorClasses::skyblue,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.u + getBaseColorRegionPercept().fieldBorderMinus.u) / 2, 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.u + getBaseColorRegionPercept().fieldBorderMinus.u) / 2, 0, 255),
  //        imageHeight
  //      );
  //      drawRegion = true;
  //    );

  //    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_v",
  //      LINE_PX
  //      (
  //        ColorClasses::red,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.v), 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.v), 0, 255),
  //        imageHeight
  //      );
  //      LINE_PX
  //      (
  //        ColorClasses::red,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.v), 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.v), 0, 255),
  //        imageHeight
  //      );
  //      LINE_PX
  //      (
  //        ColorClasses::orange,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.v + getBaseColorRegionPercept().fieldBorderMinus.v) / 2, 0, 255),
  //        0,
  //        Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.v + getBaseColorRegionPercept().fieldBorderMinus.v) / 2, 0, 255),
  //        imageHeight
  //      );
  //      drawRegion = true;
  //    );


  //if(drawRegion)
  //{
  //  LINE_PX
  //  (
  //    ColorClasses::black,
  //    256,
  //    imageHeight,
  //    256,
  //    imageHeight
  //  );
  // }

}
