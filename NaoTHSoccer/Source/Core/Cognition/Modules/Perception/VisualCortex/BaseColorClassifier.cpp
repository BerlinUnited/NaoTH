#include "BaseColorClassifier.h"

BaseColorClassifier::BaseColorClassifier()
:
  cTable(getColorTable64()),
  histogram(getHistogram()),
  coloredGrid(getColoredGrid()),
  calibCount(0) ,
  orangeBallCalibRect("OrangeBall", ColorClasses::orange, 140, 155, 180, 170),
  yellowGoalPostLeftCalibRect("YellowGoalPostLeft", ColorClasses::yellow, 60, 40, 80, 150),
  yellowGoalPostRightCalibRect("YellowGoalPostRight", ColorClasses::yellow, 240, 40, 260, 150),
  blueGoalLeftCalibRect("BlueGoalPostLeft", ColorClasses::skyblue, 60, 40, 80, 150),
  blueGoalRightCalibRect("BlueGoalPostRight", ColorClasses::skyblue, 240, 40, 260, 150),
  blueWaistBandCalibRect("BlueWaistBand", ColorClasses::blue, 200, 155, 240, 170),
  pinkWaistBandCalibRect("PinkWaistBand", ColorClasses::pink, 80, 155, 120, 170),
  whiteLinesCalibRect("WhiteLines", ColorClasses::white, 80, 170, 240, 175),
  orangeBallColorCalibrator("OrangeBall", ColorClasses::orange),
  yellowGoalColorCalibrator("YellowGoal", ColorClasses::yellow),
  blueGoalColorCalibrator("BlueGoal", ColorClasses::skyblue),
  blueWaistBandColorCalibrator("BlueWaistBand", ColorClasses::blue),
  pinkWaistBandColorCalibrator("PinkWaistBand", ColorClasses::pink),
  whiteLinesColorCalibrator("WhiteLines", ColorClasses::white)
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_ball_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_waistband_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_lines_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_colors_line", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_colors_ball", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_colors_blue_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_colors_yellow_goal", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_colors_pinkWaistBand", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:calibrate_colors_blueWaistBand", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:reset_calibration_data", " ", false);

  lastMeanY = coloredGrid.meanBrightness;
  lastMeanU = coloredGrid.meanBlue;
  lastMeanV = coloredGrid.meanRed;
  adaptationRate = 0.9;
  initPercepts();
  getBaseColorRegionPercept().setPerceptRegions();
  goalIsCalibrating = false;

  orangeBallColorCalibrator.addCalibrationRect(orangeBallCalibRect);
  yellowGoalColorCalibrator.addCalibrationRect(yellowGoalPostLeftCalibRect);
  yellowGoalColorCalibrator.addCalibrationRect(yellowGoalPostRightCalibRect);
  blueGoalColorCalibrator.addCalibrationRect(blueGoalLeftCalibRect);
  blueGoalColorCalibrator.addCalibrationRect(blueGoalRightCalibRect);
  blueWaistBandColorCalibrator.addCalibrationRect(blueWaistBandCalibRect);
  pinkWaistBandColorCalibrator.addCalibrationRect(pinkWaistBandCalibRect);
  whiteLinesColorCalibrator.addCalibrationRect(whiteLinesCalibRect);
}

void BaseColorClassifier::initPercepts()
{
  PixelT<int> chDist;
  PixelT<int> chIdx;

  regionParams.orangeBallParams.get(chIdx, chDist);    
  getBaseColorRegionPercept().orangeBall.set(chIdx, chDist);
  regionParams.blueGoalParams.get(chIdx, chDist);    
  getBaseColorRegionPercept().blueGoal.set(chIdx, chDist);
  regionParams.yellowGoalParams.get(chIdx, chDist);    
  getBaseColorRegionPercept().yellowGoal.set(chIdx, chDist);
  regionParams.pinkWaistBandParams.get(chIdx, chDist);    
  getBaseColorRegionPercept().pinkWaistBand.set(chIdx, chDist);
  regionParams.blueWaistBandParams.get(chIdx, chDist);    
  getBaseColorRegionPercept().blueWaistBand.set(chIdx, chDist);
}


void BaseColorClassifier::execute()
{
  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:reset_calibration_data",
    orangeBallColorCalibrator.reset();
    yellowGoalColorCalibrator.reset();
    blueGoalColorCalibrator.reset();
    blueWaistBandColorCalibrator.reset();
    pinkWaistBandColorCalibrator.reset();
    whiteLinesColorCalibrator.reset();
  );

  calibrateColorRegions();

  runDebugRequests();
}//end execute

void BaseColorClassifier::calibrateColorRegions()
{
  PixelT<int> chIdx;
  PixelT<int> chDist;
  chIdx.y = 0; 
  chIdx.u = 0; 
  chIdx.v = 0;
  chDist.y = 0; 
  chDist.u = 0; 
  chDist.v = 0;

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_colors_ball",
    orangeBallColorCalibrator.execute(getImage());
    orangeBallColorCalibrator.get(chIdx, chDist);
    getBaseColorRegionPercept().orangeBall.set(chIdx, chDist);
    regionParams.orangeBallParams.set(chIdx, chDist);    
    regionParams.orangeBallParams.saveToConfig();
    regionParams.orangeBallParams.syncWithConfig();
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_colors_yellow_goal",
    yellowGoalColorCalibrator.execute(getImage());
    yellowGoalColorCalibrator.get(chIdx, chDist);
    getBaseColorRegionPercept().yellowGoal.set(chIdx, chDist);
    regionParams.yellowGoalParams.set(chIdx, chDist);    
    regionParams.yellowGoalParams.saveToConfig();
    regionParams.yellowGoalParams.syncWithConfig();
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_colors_blue_goal", 
    blueGoalColorCalibrator.execute(getImage());
    blueGoalColorCalibrator.get(chIdx, chDist);
    getBaseColorRegionPercept().blueGoal.set(chIdx, chDist);
    regionParams.blueGoalParams.set(chIdx, chDist);    
    regionParams.blueGoalParams.saveToConfig();
    regionParams.blueGoalParams.syncWithConfig();
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_colors_blueWaistBand", 
    blueWaistBandColorCalibrator.execute(getImage());
    blueWaistBandColorCalibrator.get(chIdx, chDist);
    getBaseColorRegionPercept().blueWaistBand.set(chIdx, chDist);
    regionParams.blueWaistBandParams.set(chIdx, chDist);    
    regionParams.blueWaistBandParams.saveToConfig();
    regionParams.blueWaistBandParams.syncWithConfig();
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_colors_pinkWaistBand",
    pinkWaistBandColorCalibrator.execute(getImage());
    pinkWaistBandColorCalibrator.get(chIdx, chDist);
    getBaseColorRegionPercept().pinkWaistBand.set(chIdx, chDist);
    regionParams.pinkWaistBandParams.set(chIdx, chDist);    
    regionParams.pinkWaistBandParams.saveToConfig();
    regionParams.pinkWaistBandParams.syncWithConfig();
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:calibrate_colors_line",
    whiteLinesColorCalibrator.execute(getImage());
    whiteLinesColorCalibrator.get(chIdx, chDist);
    getBaseColorRegionPercept().whiteLine.set(chIdx, chDist);
    regionParams.whiteLineParams.set(chIdx, chDist);    
    regionParams.whiteLineParams.saveToConfig();
    regionParams.whiteLineParams.syncWithConfig();
  );
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

        if(getBaseColorRegionPercept().orangeBall.inside(pixel))
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

        if(getBaseColorRegionPercept().yellowGoal.inside(pixel))
        {
          POINT_PX(ColorClasses::yellow, x, y);
        }

        if(getBaseColorRegionPercept().blueGoal.inside(pixel))
        {
          POINT_PX(ColorClasses::skyblue, x, y);
        }

      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_waistband_in_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().pinkWaistBand.inside(pixel))
        {
          POINT_PX(ColorClasses::pink, x, y);
        }

        if(getBaseColorRegionPercept().blueWaistBand.inside(pixel))
        {
          POINT_PX(ColorClasses::blue, x, y);
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

        if(getBaseColorRegionPercept().whiteLine.inside(pixel))
        {
          POINT_PX(ColorClasses::white, x, y);
        }

      }
    }
  );
}
