#include "ColorCalibrator.h"

ColorCalibrator::ColorCalibrator()
:
  leftGoalPost("GoalPostLeft", ColorClasses::yellow, 60, 40, 80, 150),
  rightGoalPost("GoalPostRight", ColorClasses::skyblue, 240, 40, 260, 150),
  orangeBall("OrangeBall", ColorClasses::orange, 140, 155, 180, 170),
  pinkWaistBand("PinkWaistBand", ColorClasses::pink, 80, 155, 120, 170),
  blueWaistBand("BlueWaistBand", ColorClasses::blue, 200, 155, 240, 170),
  leftField("LeftField", ColorClasses::green, 40, 200, 100, 220),
  rightField("RightField", ColorClasses::green, 220, 200, 280, 220)
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:calibrate_color_regions", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:calibrate_color_region_field", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:calibrate_color_region_ball", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:calibrate_color_region_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:calibrate_color_region_pinkWaistBand", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:calibrate_color_region_blueWaistBand", " ", false);
  
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_calibrated_colors", " ", true);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_calibrated_colors_field", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_calibrated_colors_ball", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_calibrated_colors_blue_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_calibrated_colors_yellow_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_calibrated_colors_pinkWaistBand", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_calibrated_colors_blueWaistBand", " ", false);
  
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_field", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_ball", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_blue_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_yellow_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_pinkWaistBand", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:show_percept_colors_blueWaistBand", " ", false);
  
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_field", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_ball", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_blue_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_yellow_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_pinkWaistBand", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:set_percept_colors_blueWaistBand", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:ColorCalibrator:reset_calibration", " ", false);

  initHistograms(ballHistColorChannel, ballHistDifference);
  initHistograms(goalHistColorChannel, goalHistDifference);
  initHistograms(pinkWaistBandHistColorChannel, pinkWaistBandHistDifference);
  initHistograms(blueWaistBandHistColorChannel, blueWaistBandHistDifference);
  initHistograms(fieldHistColorChannel, fieldHistDifference);
  initHistograms(linesHistColorChannel, linesHistDifference);

  PixelT<int> chDist;
  PixelT<int> chIdx;
  getBaseColorRegionPercept().orangeBall.get(chIdx, chDist);
  orangeBallParams.set(chIdx, chDist);    
  getBaseColorRegionPercept().blueGoal.get(chIdx, chDist);
  blueGoalParams.set(chIdx, chDist);    
  getBaseColorRegionPercept().yellowGoal.get(chIdx, chDist);
  yellowGoalParams.set(chIdx, chDist);    
  getBaseColorRegionPercept().pinkWaistBand.get(chIdx, chDist);
  pinkWaistBandParams.set(chIdx, chDist);    
  getBaseColorRegionPercept().blueWaistBand.get(chIdx, chDist);
  blueWaistBandParams.set(chIdx, chDist);    
}

void  ColorCalibrator::initHistograms
(
  vector<vector<double> >& histColorChannel,
  vector<vector<double> >& histDifference
)
{
  histColorChannel.clear();
  histDifference.clear();

  vector<double> VminusU(512);
  histDifference.push_back(VminusU);
  vector<double> UminusY(512);
  histDifference.push_back(UminusY);
  vector<double> VminusY(512);
  histDifference.push_back(VminusY);

  vector<double> Y(256);
  histColorChannel.push_back(Y);
  vector<double> U(256);
  histColorChannel.push_back(U);
  vector<double> V(256);
  histColorChannel.push_back(V);
}

void ColorCalibrator::execute()
{  
  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:calibrate_color_regions",
    calibrateColorRegions();
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:reset_calibration",
    initHistograms(ballHistColorChannel, ballHistDifference);
    initHistograms(goalHistColorChannel, goalHistDifference);
    initHistograms(pinkWaistBandHistColorChannel, pinkWaistBandHistDifference);
    initHistograms(blueWaistBandHistColorChannel, blueWaistBandHistDifference);
    initHistograms(fieldHistColorChannel, fieldHistDifference);
    initHistograms(linesHistColorChannel, linesHistDifference);
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:set_percept_colors_field",
    PixelT<int> chDist;
    PixelT<int> chIdx;
    greenFieldParams.get(chIdx, chDist); 

    getFieldColorPercept().maxY2Set = Math::clamp<int>(chIdx.y + chDist.y, 0, 255);
    getFieldColorPercept().maxU2Set = Math::clamp<int>(chIdx.u + chDist.u, 0, 255);
    getFieldColorPercept().distV2Set = chDist.v;

  );

  //DEBUG_REQUEST("ImageProcessor:ColorCalibrator:set_percept_colors_lines",
  //  PixelT<int> chDist;
  //  PixelT<int> chIdx;
  //  whiteLinesParams.get(chIdx, chDist);    
  //  getBaseColorRegionPercept().orangeBall.set(chIdx, chDist);
  //);

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:set_percept_colors_ball",
    PixelT<int> chDist;
    PixelT<int> chIdx;
    orangeBallParams.get(chIdx, chDist);    
    getBaseColorRegionPercept().orangeBall.set(chIdx, chDist);
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:set_percept_colors_blue_goal",
    PixelT<int> chDist;
    PixelT<int> chIdx;
    blueGoalParams.get(chIdx, chDist);    
    getBaseColorRegionPercept().blueGoal.set(chIdx, chDist);
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:set_percept_colors_yellow_goal",
    PixelT<int> chDist;
    PixelT<int> chIdx;
    yellowGoalParams.get(chIdx, chDist);    
    getBaseColorRegionPercept().yellowGoal.set(chIdx, chDist);
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:set_percept_colors_pinkWaistBand",
    PixelT<int> chDist;
    PixelT<int> chIdx;
    pinkWaistBandParams.get(chIdx, chDist);    
    getBaseColorRegionPercept().pinkWaistBand.set(chIdx, chDist);
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:set_percept_colors_blueWaistBand",
    PixelT<int> chDist;
    PixelT<int> chIdx;
    blueWaistBandParams.get(chIdx, chDist);    
    getBaseColorRegionPercept().blueWaistBand.set(chIdx, chDist);
  );

  runDebugRequests();

}//end execute

void ColorCalibrator::calibrateColorRegions()
{
  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:calibrate_color_region_field",
    calibrateColorRegionField();
  );
  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:calibrate_color_region_ball",
    calibrateColorRegionBall();
  );
  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:calibrate_color_region_goal",
    calibrateColorRegionGoal();
  );
  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:calibrate_color_region_pinkWaistBand",
    calibrateColorRegionPinkWaistBand();
  );
  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:calibrate_color_region_blueWaistBand",
    calibrateColorRegionBlueWaistBand();
  );
}

void ColorCalibrator::calibrateColorRegionField()
{
  //green field
  //leftField.draw();
  //rightField.draw();

  CalibrationRect fieldCalibRect("Field", ColorClasses::green, 0, getImage().height() / 2, getImage().width() - 1, getImage().height() - 2);
  fieldCalibRect.draw();

  /// field
  getAverageDistances(fieldCalibRect, fieldHistColorChannel, fieldHistDifference, greenFieldParams); 
  //getAverageDistances2(leftField, rightField, fieldHistColorChannel, fieldHistDifference, fieldParams);
  //PLOT("BCCT:avgGreenV-U", regionParams.fieldParams.ccdIdx.x);
  //PLOT("BCCT:avgGreenU-Y", regionParams.fieldParams.ccdIdx.y);
  //PLOT("BCCT:avgGreenV-Y", regionParams.fieldParams.ccdIdx.z);  
}

void ColorCalibrator::calibrateColorRegionLines()
{
}

void ColorCalibrator::calibrateColorRegionBall()
{
  //ball
  orangeBall.draw();

  /// orange ball
  getAverageDistances(orangeBall, ballHistColorChannel, ballHistDifference, orangeBallParams);
  //PLOT("BCCT:avgOrangeBallV-U", regionParams.orangeBallParams.ccdIdx.x);
  //PLOT("BCCT:avgOrangeBallU-Y", regionParams.orangeBallParams.ccdIdx.y);
  //PLOT("BCCT:avgOrangeBallV-Y", regionParams.orangeBallParams.ccdIdx.z);
}

void ColorCalibrator::calibrateColorRegionGoal()
{
  //goalPosts
  leftGoalPost.draw();
  rightGoalPost.draw();

  /// goals
  colorPixel ccdIdx; ccdIdx.VminusU = 0; ccdIdx.UminusY = 0; ccdIdx.VminusY = 0;
  colorPixel ccdDist; ccdDist.VminusU = 0; ccdDist.UminusY = 0; ccdDist.VminusY = 0;
  PixelT<int> ccIdx; ccIdx.y = 0; ccIdx.u = 0; ccIdx.v = 0;
  PixelT<int> ccDist; ccDist.y = 0; ccDist.u = 0; ccDist.v = 0;

  getAverageDistances2(leftGoalPost, rightGoalPost, goalHistColorChannel, goalHistDifference, ccdIdx, ccdDist, ccIdx, ccDist);

  calculateRegions(goalHistColorChannel, goalHistDifference, ccdIdx, ccdDist, ccIdx, ccDist);

  if(ccdIdx.VminusU > 0)
  {
    yellowGoalParams.set(ccdIdx, ccdDist);
    yellowGoalParams.set(ccIdx, ccDist);
    //yellowGoalParams.saveToConfig();
    //PLOT("BCCT:avgYellowGoalV-U", ccdIdx.x);
    //PLOT("BCCT:avgYellowGoalU-Y", ccdIdx.y);
    //PLOT("BCCT:avgYellowGoalV-Y", ccdIdx.z);
  }
  else
  {
    blueGoalParams.set(ccdIdx, ccdDist);
    blueGoalParams.set(ccIdx, ccDist);
    //blueGoalParams.saveToConfig();
    //PLOT("BCCT:avgBlueGoalV-U", ccdIdx.x);
    //PLOT("BCCT:avgBlueGoalU-Y", ccdIdx.y);
    //PLOT("BCCT:avgBlueGoalV-Y", ccdIdx.z);
  }
}

void ColorCalibrator::calibrateColorRegionPinkWaistBand()
{
  //pink waistband
  pinkWaistBand.draw();

  /// pink waist band
  getAverageDistances(pinkWaistBand, pinkWaistBandHistColorChannel, pinkWaistBandHistDifference, pinkWaistBandParams);
  //PLOT("BCCT:avgPinkWaistBandV-U", regionParams.pinkWaistBandParams.ccdIdx.x);
  //PLOT("BCCT:avgPinkWaistBandU-Y", regionParams.pinkWaistBandParams.ccdIdx.y);
  //PLOT("BCCT:avgPinkWaistBandV-Y", regionParams.pinkWaistBandParams.ccdIdx.z);
}

void ColorCalibrator::calibrateColorRegionBlueWaistBand()
{
  //blue waistband
  blueWaistBand.draw();

  /// blue waist band
  getAverageDistances(blueWaistBand, blueWaistBandHistColorChannel, blueWaistBandHistDifference, blueWaistBandParams);
  //PLOT("BCCT:avgBlueWaistBandV-U", regionParams.blueWaistBandParams.ccdIdx.x);
  //PLOT("BCCT:avgBlueWaistBandU-Y", regionParams.blueWaistBandParams.ccdIdx.y);
  //PLOT("BCCT:avgBlueWaistBandV-Y", regionParams.blueWaistBandParams.ccdIdx.z);
}


void ColorCalibrator::getAverageDistances
(
  CalibrationRect& cRect,
  vector<vector<double> >& histColorChannel,
  vector<vector<double> >& histDifference,
  ColorRegion& param
)
{
  Vector2<int> ll = cRect.lowerLeft;
  Vector2<int> ur = cRect.upperRight;

  colorPixel ccdIdx; ccdIdx.VminusU = 0; ccdIdx.UminusY = 0; ccdIdx.VminusY = 0;
  colorPixel ccdDist; ccdDist.VminusU = 0; ccdDist.UminusY = 0; ccdDist.VminusY = 0;
  PixelT<int> ccIdx; ccIdx.y = 0; ccIdx.u = 0; ccIdx.v = 0;
  PixelT<int> ccDist; ccDist.y = 0; ccDist.u = 0; ccDist.v = 0;

  int x = (ur.x + ll.x) / 2;
  for(int y = ll.y; y <= ur.y; y++)
  {
    getAverageDistancesLoopBody(x, y, histColorChannel, histDifference);
  }

  int y = (ur.y + ll.y) / 2;
  for(int x = ll.x; x <= ur.x; x++)
  {
    getAverageDistancesLoopBody(x, y, histColorChannel, histDifference);
  }

  calculateRegions(histColorChannel, histDifference, ccdIdx, ccdDist, ccIdx, ccDist);

  param.set(ccdIdx, ccdDist); 
  param.set(ccIdx, ccDist);
  //param.saveToConfig();

}

void ColorCalibrator::getAverageDistances2
(
  CalibrationRect& cRect1,
  CalibrationRect& cRect2,
  vector<vector<double> >& histColorChannel,
  vector<vector<double> >& histDifference,
  ColorRegion& param
)
{
  colorPixel ccdIdx;
  colorPixel ccdDist;
  PixelT<int> ccIdx; ccIdx.y = 0; ccIdx.u = 0; ccIdx.v = 0;
  PixelT<int> ccDist; ccDist.y = 0; ccDist.u = 0; ccDist.v = 0;

  getAverageDistances2(cRect1, cRect2, histColorChannel, histDifference, ccdIdx, ccdDist, ccIdx, ccDist);

  param.set(ccdIdx, ccdDist);
  param.set(ccIdx, ccDist);
  //param.saveToConfig();
}

void ColorCalibrator::getAverageDistances2
(
  CalibrationRect& cRect1,
  CalibrationRect& cRect2,
  vector<vector<double> >& histColorChannel,
  vector<vector<double> >& histDifference,
  colorPixel& ccdIdx,
  colorPixel& ccdDist,
  PixelT<int>& ccIdx,
  PixelT<int>& ccDist
)
{
  Vector2<int> ll = cRect1.lowerLeft;
  Vector2<int> ur = cRect1.upperRight;

  int x = (ur.x + ll.x) / 2;
  for(int y = ll.y; y <= ur.y; y++)
  {
    getAverageDistancesLoopBody(x, y, histColorChannel, histDifference);
  }

  int y = (ur.y + ll.y) / 2;
  for(int x = ll.x; x <= ur.x; x++)
  {
    getAverageDistancesLoopBody(x, y, histColorChannel, histDifference);
  }

  ll = cRect2.lowerLeft;
  ur = cRect2.upperRight;
  x = (ur.x + ll.x) / 2;
  for(int y = ll.y; y <= ur.y; y++)
  {
    getAverageDistancesLoopBody(x, y, histColorChannel, histDifference);
  }

  y = (ur.y + ll.y) / 2;
  for(int x = ll.x; x <= ur.x; x++)
  {
    getAverageDistancesLoopBody(x, y, histColorChannel, histDifference);
  }

}

void ColorCalibrator::getAverageDistancesLoopBody
(
  int& x,
  int& y,
  vector<vector<double> >& histColorChannel,
  vector<vector<double> >& histDifference
)
{
  Pixel pixel = getImage().get(x, y);
  
  colorPixel dist;
  dist.VminusU = pixel.v - pixel.u;
  dist.UminusY = pixel.u - pixel.y;
  dist.VminusY = pixel.v - pixel.y;

  histDifference[0][255 + dist.VminusU]++;
  histDifference[1][255 + dist.UminusY]++;
  histDifference[2][255 + dist.VminusY]++;
  histColorChannel[0][pixel.y]++;
  histColorChannel[1][pixel.u]++;
  histColorChannel[2][pixel.v]++;
}

void ColorCalibrator::calculateRegions
(
  vector<vector<double> >& histColorChannel,
  vector<vector<double> >& histDifference,
  colorPixel& ccdIdx,
  colorPixel& ccdDist,
  PixelT<int>& ccIdx,
  PixelT<int>& ccDist
)
{
  Vector3<double> m1;
  Vector3<double> m2;
  Vector3<double> z2;
  Vector3<double> sigma;

  Vector3<double> sum;

  vector<vector<double> > histColor;
  vector<vector<double> > histDiff;

  initHistograms(histColor, histDiff);

  for(int i = 0; i < 512; i++)
  {
    sum.x += histDifference[0][i];
    sum.y += histDifference[1][i];
    sum.z += histDifference[2][i];
  }

  for(int i = 0; i < 512; i++)
  {
    histDiff[0][i] = histDifference[0][i] / sum.x;
    histDiff[1][i] = histDifference[1][i] / sum.y;
    histDiff[2][i] = histDifference[2][i] / sum.z;
  }

  for(int i = 0; i < 512; i++)
  {
     double temp =  (i - 255) * histDiff[0][i];
     m1.x += temp;
     m2.x += (i - 255) * temp;
     temp = (i - 255) * histDiff[1][i];
     m1.y += temp;
     m2.y += (i - 255) * temp;
     temp =  (i - 255) * histDiff[2][i];
     m1.z += temp;
     m2.z += (i - 255) * temp;
  }
  z2.x = m2.x - (m1.x * m1.x);
  z2.y = m2.y - (m1.y * m1.y);
  z2.z = m2.z - (m1.z * m1.z);

  sigma.x = sqrt(fabs(z2.x));
  sigma.y = sqrt(fabs(z2.y));
  sigma.z = sqrt(fabs(z2.z));

  ccdIdx.VminusU = (int) m1.x;
  ccdIdx.UminusY = (int) m1.y;
  ccdIdx.VminusY = (int) m1.z;
  
  ccdDist.VminusU = (int) (sigma.x * 1.4);
  ccdDist.UminusY = (int) (sigma.y * 1.4);
  ccdDist.VminusY = (int) (sigma.z * 1.4);

  m1.x = 0;
  m1.y = 0;
  m1.z = 0;
  m2.x = 0;
  m2.y = 0;
  m2.z = 0;
  z2.x = 0;
  z2.y = 0;
  z2.z = 0;
  sigma.x = 0;
  sigma.y = 0;
  sigma.z = 0;

  sum.x = 0;
  sum.y = 0;
  sum.z = 0;
  for(int i = 0; i < 256; i++)
  {
    sum.x += histColorChannel[0][i];
    sum.y += histColorChannel[1][i];
    sum.z += histColorChannel[2][i];
  }

  for(int i = 0; i < 256; i++)
  {
    histColor[0][i] = histColorChannel[0][i] / sum.x;
    histColor[1][i] = histColorChannel[1][i] / sum.y;
    histColor[2][i] = histColorChannel[2][i] / sum.z;
  }

  for(int i = 0; i < 256; i++)
  {
     double temp = i * histColor[0][i];
     m1.x += temp;
     m2.x += i * temp;
     temp = i * histColor[1][i];
     m1.y += temp;
     m2.y += i * temp;
     temp = i * histColor[2][i];
     m1.z += temp;
     m2.z += i * temp;
  }
  z2.x = m2.x - (m1.x * m1.x);
  z2.y = m2.y - (m1.y * m1.y);
  z2.z = m2.z - (m1.z * m1.z);

  sigma.x = sqrt(fabs(z2.x));
  sigma.y = sqrt(fabs(z2.y));
  sigma.z = sqrt(fabs(z2.z));

  ccIdx.y = (int) m1.x;
  ccIdx.u = (int) m1.y;
  ccIdx.v = (int) m1.z;
  ccDist.y = (int) (sigma.x * 1.4);
  ccDist.u = (int) (sigma.y * 1.4);
  ccDist.v = (int) (sigma.z * 1.4);
}

void ColorCalibrator::runDebugRequests()
{
  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_calibrated_colors",
    bool showField = false;
    greenFieldParams.sync();
    DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_calibrated_colors_field",
      showField = true;
    );
    bool showBall = false;
    orangeBallParams.sync();
    DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_calibrated_colors_ball",
      showBall = true;
    );
    bool showBlueGoal = false;
    blueGoalParams.sync();
    DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_calibrated_colors_blue_goal",
      showBlueGoal = true;
    );
    bool showYellowGoal = false;
    yellowGoalParams.sync();
    DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_calibrated_colors_yellow_goal",
      showYellowGoal = true;
    );
    bool showPinkWaistBand = false;
    pinkWaistBandParams.sync();
    DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_calibrated_colors_pinkWaistBand",
      showPinkWaistBand = true;
    );
    bool showBlueWaistBand = false;
    blueWaistBandParams.sync();
    DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_calibrated_colors_blueWaistBand",
      showBlueWaistBand = true;
    );
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(showField && greenFieldParams.inside(pixel) )
        {
          POINT_PX(ColorClasses::green, x, y);
        }

        if(showBall && orangeBallParams.inside(pixel))
        {
          POINT_PX(ColorClasses::red, x, y);
        }

        if(showBlueGoal && blueGoalParams.inside(pixel))
        {
          POINT_PX(ColorClasses::skyblue, x, y);
        }

        if(showYellowGoal && yellowGoalParams.inside(pixel) )
        {
          POINT_PX(ColorClasses::yellow, x, y);
        }

        if(showPinkWaistBand && pinkWaistBandParams.inside(pixel))
        {
          POINT_PX(ColorClasses::pink, x, y);
        }

        if(showBlueWaistBand && blueWaistBandParams.inside(pixel) )
        {
          POINT_PX(ColorClasses::blue, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_percept_colors_ball",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isOrangeBall(pixel))
        {
          POINT_PX(ColorClasses::red, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_percept_colors_yellow_goal",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isYellowGoal(pixel))
        {
          POINT_PX(ColorClasses::yellow, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_percept_colors_blue_goal",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isBlueGoal(pixel))
        {
          POINT_PX(ColorClasses::yellow, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_percept_colors_pinkWaistBand",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isPinkWaistBand(pixel.y, pixel.u, pixel.v))
        {
          POINT_PX(ColorClasses::white, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_percept_colors_blueWaistBand",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isBlueWaistBand(pixel))
        {
          POINT_PX(ColorClasses::gray, x, y);
        }
      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:ColorCalibrator:show_percept_colors_field",
    for(unsigned int x = 0; x < getImage().width(); x++)
    {
      for(unsigned int y = 0; y < getImage().height(); y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getFieldColorPercept().isFieldColor(pixel))
        {
          POINT_PX(ColorClasses::green, x, y);
        }

      }
    }
  );
}
