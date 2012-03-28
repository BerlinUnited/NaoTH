#include "BaseColorClassifier.h"

BaseColorClassifier::BaseColorClassifier()
:
  cTable(getColorTable64()),
  histogram(getHistogram()),
  coloredGrid(getColoredGrid())
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_ball_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_lines_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_field_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_simple_goal_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_image_2", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_corrected_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:initPercept", " ", false);

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
  getBaseColorRegionPercept().setPerceptRegions();
  goalIsCalibrating = false;
}

void BaseColorClassifier::initPercept()
{
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
  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:initPercept",
    initPercept();
  );

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
  getBaseColorRegionPercept().goalVUdistance = regionParams.goalVUdistance;
  getBaseColorRegionPercept().goalVUdistanceMin = regionParams.goalVUdistanceMin;
  getBaseColorRegionPercept().goalVUdistanceMax = regionParams.goalVUdistanceMax;

  getBaseColorRegionPercept().setPerceptRegions();

  getBaseColorRegionPercept().lineBorderMinus.y = Math::clamp<double>(getFieldColorPercept().maxWeightedIndexY + getFieldColorPercept().distY, 0.0, 255.0);
  getBaseColorRegionPercept().lineBorderPlus.y = 255.0;

  getBaseColorRegionPercept().lineColorRegion.set(getBaseColorRegionPercept().lineBorderMinus, getBaseColorRegionPercept().lineBorderPlus);
  
  getBaseColorRegionPercept().goalBorderMinus.y = Math::clamp<double>(getFieldColorPercept().maxWeightedIndexY/* + getFieldColorPercept().distY*/, 0.0, 255.0);

  getBaseColorRegionPercept().goalColorRegion.set(getBaseColorRegionPercept().goalBorderMinus, getBaseColorRegionPercept().goalBorderPlus);

  getBaseColorRegionPercept().lastUpdated = getFrameInfo();

  runDebugRequests();
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

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_simple_goal_in_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isYellowSimple(pixel))
        {
          POINT_PX(ColorClasses::yellowOrange, x, y);
        }

      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_goal_in_corrected_image",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().getCorrected(x, y);

        if(getBaseColorRegionPercept().isYellow(pixel))
        {
          POINT_PX(ColorClasses::yellowOrange, x, y);
        }

      }
    }
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:set_goal_in_image_2",
    for(int x = 0; x < imageWidth; x++)
    {
      for(int y = 0; y < imageHeight; y++)
      {
        const Pixel& pixel = getImage().get(x, y);

        if(getBaseColorRegionPercept().isYellow2(pixel))
        {
          POINT_PX(ColorClasses::yellowOrange, x, y);
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

        if(getBaseColorRegionPercept().isWhite(pixel))
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

        if(getBaseColorRegionPercept().isGreenOrBlue(pixel))
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
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.y), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.y), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.y), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.y), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::white,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.y + getBaseColorRegionPercept().ballBorderMinus.y) / 2, 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.y + getBaseColorRegionPercept().ballBorderMinus.y) / 2, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_u",
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.u), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.u), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.u), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.u), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::skyblue,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.u + getBaseColorRegionPercept().ballBorderMinus.u) / 2, 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.u + getBaseColorRegionPercept().ballBorderMinus.u) / 2, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_v",
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.v), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderMinus.v), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.v), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.v), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::orange,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.v + getBaseColorRegionPercept().ballBorderMinus.v) / 2, 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballBorderPlus.v + getBaseColorRegionPercept().ballBorderMinus.v) / 2, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_y",
      LINE_PX
      (
        ColorClasses::gray,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.y), 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.y), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::gray,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.y), 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.y), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::white,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.y + getBaseColorRegionPercept().goalBorderMinus.y) / 2, 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.y + getBaseColorRegionPercept().goalBorderMinus.y) / 2, 0, 255),
        imageHeight
      );
      drawRegion = true;
    );

    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_u",
      LINE_PX
      (
        ColorClasses::blue,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.u), 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.u), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::blue,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.u), 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.u), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::skyblue,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.u + getBaseColorRegionPercept().goalBorderMinus.u) / 2, 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.u + getBaseColorRegionPercept().goalBorderMinus.u) / 2, 0, 255),
        imageHeight
      );
      drawRegion = true;
    );

    DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_v",
      LINE_PX
      (
        ColorClasses::red,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.v), 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderMinus.v), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::red,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.v), 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.v), 0, 255),
        imageHeight
      );
      LINE_PX
      (
        ColorClasses::orange,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.v + getBaseColorRegionPercept().goalBorderMinus.v) / 2, 0, 255),
        0,
        Math::clamp((int) (getBaseColorRegionPercept().goalBorderPlus.v + getBaseColorRegionPercept().goalBorderMinus.v) / 2, 0, 255),
        imageHeight
      );
      drawRegion = true;
    );

      DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_y",
        LINE_PX
        (
          ColorClasses::gray,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.y), 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.y), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::gray,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.y), 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.y), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::white,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.y + getBaseColorRegionPercept().fieldBorderMinus.y) / 2, 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.y + getBaseColorRegionPercept().fieldBorderMinus.y) / 2, 0, 255),
          imageHeight
        );
        drawRegion = true;
      );

      DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_u",
        LINE_PX
        (
          ColorClasses::blue,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.u), 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.u), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::blue,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.u), 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.u), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::skyblue,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.u + getBaseColorRegionPercept().fieldBorderMinus.u) / 2, 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.u + getBaseColorRegionPercept().fieldBorderMinus.u) / 2, 0, 255),
          imageHeight
        );
        drawRegion = true;
      );

      DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:field_v",
        LINE_PX
        (
          ColorClasses::red,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.v), 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderMinus.v), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::red,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.v), 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.v), 0, 255),
          imageHeight
        );
        LINE_PX
        (
          ColorClasses::orange,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.v + getBaseColorRegionPercept().fieldBorderMinus.v) / 2, 0, 255),
          0,
          Math::clamp((int) (getBaseColorRegionPercept().fieldBorderPlus.v + getBaseColorRegionPercept().fieldBorderMinus.v) / 2, 0, 255),
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
