#include "BaseColorClassifier.h"

BaseColorClassifier::BaseColorClassifier()
{
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_ball_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_goal_in_image", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:set_lines_in_image", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_y", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_cb", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:ball_cr", " ", false);

  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_y", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_cb", " ", false);
  DEBUG_REQUEST_REGISTER("ImageProcessor:BaseColorClassifier:goal_cr", " ", false);

  lastMeanY = getColoredGrid().meanBrightness;
  lastMeanCb = getColoredGrid().meanBlue;
  lastMeanCr = getColoredGrid().meanRed;
  adaptationRate = 0.01;
}

void BaseColorClassifier::execute()
{
  double diff = fabs(getColoredGrid().meanBrightness - lastMeanY);
  if(diff > 5.0)
  {
    meanY.add(getColoredGrid().meanBrightness);
    getBaseColorRegionPercept().meanY = meanY.getAverage();
    getBaseColorRegionPercept().diffY = diff;
    lastMeanY = getColoredGrid().meanBrightness;
  }
  diff = fabs(getColoredGrid().meanBlue - lastMeanCb);
  if(diff > 5.0)
  {
    meanCb.add(getColoredGrid().meanBlue);
    getBaseColorRegionPercept().meanCb = meanCb.getAverage();
    getBaseColorRegionPercept().diffCb = diff;
    lastMeanCb = getColoredGrid().meanBlue;
  }
  diff = fabs(getColoredGrid().meanRed - lastMeanCr);
  if(diff > 5.0)
  {
    meanCr.add(getColoredGrid().meanRed);
    getBaseColorRegionPercept().meanCr = meanCr.getAverage();
    getBaseColorRegionPercept().diffCr = diff;
    lastMeanCr = getColoredGrid().meanRed;
  }

  unsigned int maxBallIndexY = 0.0;
  unsigned int maxBallIndexCb = 0.0;
  unsigned int maxBallIndexCr = 0.0;
  double maxBallY = 0.0;
  double maxBallCb = 0.0;
  double maxBallCr = 0.0;

  unsigned int maxGoalIndexY = 0.0;
  unsigned int maxGoalIndexCb = 0.0;
  unsigned int maxGoalIndexCr = 0.0;
  double maxGoalY = 0.0;
  double maxGoalCb = 0.0;
  double maxGoalCr = 0.0;

  unsigned int maxLineIndexY = 0.0;
  unsigned int maxLineIndexCb = 0.0;
  unsigned int maxLineIndexCr = 0.0;
  double maxLineY = 0.0;
  double maxLineCb = 0.0;
  double maxLineCr = 0.0;

  MODIFY("BaseColorClassifier:adaptationRate", adaptationRate);

  MODIFY("BaseColorClassifier:distBallY", getBaseColorRegionPercept().distBallY);
  MODIFY("BaseColorClassifier:distBallCb", getBaseColorRegionPercept().distBallCb); 
  MODIFY("BaseColorClassifier:distBallCr", getBaseColorRegionPercept().distBallCr);

  MODIFY("BaseColorClassifier:ballIndexY", getBaseColorRegionPercept().ballIndexY);
  MODIFY("BaseColorClassifier:ballIndexCb", getBaseColorRegionPercept().ballIndexCb);
  MODIFY("BaseColorClassifier:ballIndexCr", getBaseColorRegionPercept().ballIndexCr);

  MODIFY("BaseColorClassifier:distGoalY", getBaseColorRegionPercept().distGoalY);
  MODIFY("BaseColorClassifier:distGoalCb", getBaseColorRegionPercept().distGoalCb);
  MODIFY("BaseColorClassifier:distGoalCr", getBaseColorRegionPercept().distGoalCr);

  MODIFY("BaseColorClassifier:goalIndexY", getBaseColorRegionPercept().goalIndexY);
  MODIFY("BaseColorClassifier:goalIndexCb", getBaseColorRegionPercept().goalIndexCb);
  MODIFY("BaseColorClassifier:goalIndexCr", getBaseColorRegionPercept().goalIndexCr);

  MODIFY("BaseColorClassifier:distLineY", getBaseColorRegionPercept().distLineY);
  MODIFY("BaseColorClassifier:distLineCb", getBaseColorRegionPercept().distLineCb);
  MODIFY("BaseColorClassifier:distLineCr", getBaseColorRegionPercept().distLineCr);

  MODIFY("BaseColorClassifier:lineIndexY", getBaseColorRegionPercept().lineIndexY);
  MODIFY("BaseColorClassifier:lineIndexCb", getBaseColorRegionPercept().lineIndexCb);
  MODIFY("BaseColorClassifier:lineIndexCr", getBaseColorRegionPercept().lineIndexCr);

  for(int i = 0; i < COLOR_CHANNEL_VALUE_COUNT; i++)
  {
    if(maxBallY < getHistogram().colorChannelHistogramBall[0][i])
    {
      maxBallY = getHistogram().colorChannelHistogramBall[0][i];
      maxBallIndexY = i;
    }
    if(maxBallCb < getHistogram().colorChannelHistogramBall[1][i])
    {
      maxBallCb = getHistogram().colorChannelHistogramBall[1][i];
      maxBallIndexCb = i;
    }
    if(maxBallCr < getHistogram().colorChannelHistogramBall[2][i])
    {
      maxBallCr = getHistogram().colorChannelHistogramBall[2][i];
      maxBallIndexCr = i;
    }

    if(maxGoalY < getHistogram().colorChannelHistogramGoal[0][i])
    {
      maxGoalY = getHistogram().colorChannelHistogramGoal[0][i];
      maxGoalIndexY = i;
    }
    if(maxGoalCb < getHistogram().colorChannelHistogramGoal[1][i])
    {
      maxGoalCb = getHistogram().colorChannelHistogramGoal[1][i];
      maxGoalIndexCb = i;
    }
    if(maxGoalCr < getHistogram().colorChannelHistogramGoal[2][i])
    {
      maxGoalCr = getHistogram().colorChannelHistogramGoal[2][i];
      maxGoalIndexCr = i;
    }

    if(maxLineY < getHistogram().colorChannelHistogramLine[0][i])
    {
      maxLineY = getHistogram().colorChannelHistogramLine[0][i];
      maxLineIndexY = i;
    }
    if(maxLineCb < getHistogram().colorChannelHistogramLine[1][i])
    {
      maxLineCb = getHistogram().colorChannelHistogramLine[1][i];
      maxLineIndexCb = i;
    }
    if(maxLineCr < getHistogram().colorChannelHistogramLine[2][i])
    {
      maxLineCr = getHistogram().colorChannelHistogramLine[2][i];
      maxLineIndexCr = i;
    }
  }

  if(fabs(maxBallIndexY - BALL_IDX_Y) < getBaseColorRegionPercept().diffY / 2)
  {
    getBaseColorRegionPercept().ballIndexY =
      (1 - adaptationRate) * getBaseColorRegionPercept().ballIndexY + adaptationRate * maxBallIndexY;
  }
  if(fabs(maxBallIndexCb - BALL_IDX_CB) < getBaseColorRegionPercept().diffCb / 2)
  {
    getBaseColorRegionPercept().ballIndexCb =
      (1 - adaptationRate) * getBaseColorRegionPercept().ballIndexCb + adaptationRate * maxBallIndexCb;
  }
  if(fabs(maxBallIndexCr - BALL_IDX_CR) < getBaseColorRegionPercept().diffCr / 2)
  {
    getBaseColorRegionPercept().ballIndexCr =
      (1 - adaptationRate) * getBaseColorRegionPercept().ballIndexCr + adaptationRate * maxBallIndexCr;
  }

  if(fabs(maxGoalIndexY - GOAL_IDX_Y) < getBaseColorRegionPercept().diffY / 2)
  {
    getBaseColorRegionPercept().goalIndexY =
      (1 - adaptationRate) * getBaseColorRegionPercept().goalIndexY + adaptationRate * maxGoalIndexY;
  }
  if(fabs(maxGoalIndexCb - GOAL_IDX_CB) < getBaseColorRegionPercept().diffCb / 2)
  {
    getBaseColorRegionPercept().goalIndexCb =
      (1 - adaptationRate) * getBaseColorRegionPercept().goalIndexCb + adaptationRate * maxGoalIndexCb;
  }
  if(fabs(maxGoalIndexCr - GOAL_IDX_CR) < getBaseColorRegionPercept().diffCr / 2)
  {
    getBaseColorRegionPercept().goalIndexCr =
      (1 - adaptationRate) * getBaseColorRegionPercept().goalIndexCr + adaptationRate * maxGoalIndexCr;
  }

  if(fabs(maxLineIndexY - LINE_IDX_CR) < getBaseColorRegionPercept().diffY / 2)
  {
    getBaseColorRegionPercept().lineIndexY =
      (1 - adaptationRate) * getBaseColorRegionPercept().lineIndexY + adaptationRate * maxLineIndexY;
  }
  if(fabs(maxLineIndexCb - LINE_IDX_CR) < getBaseColorRegionPercept().diffCb / 2)
  {
    getBaseColorRegionPercept().lineIndexCb =
      (1 - adaptationRate) * getBaseColorRegionPercept().lineIndexCb + adaptationRate * maxLineIndexCb;
  }
  if(fabs(maxLineIndexCr - LINE_IDX_CR) < getBaseColorRegionPercept().diffCr / 2)
  {
    getBaseColorRegionPercept().lineIndexCr =
      (1 - adaptationRate) * getBaseColorRegionPercept().lineIndexCr + adaptationRate * maxLineIndexCr;
  }

  getBaseColorRegionPercept().lastUpdated = getFrameInfo();

  runDebugRequests();
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

  bool drawRegion = false;

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_y",
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexY - getBaseColorRegionPercept().distBallY), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexY - getBaseColorRegionPercept().distBallY), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexY + getBaseColorRegionPercept().distBallY), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexY + getBaseColorRegionPercept().distBallY), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::white,
      Math::clamp((int) getBaseColorRegionPercept().ballIndexY, 0, 255),
      0,
      Math::clamp((int) getBaseColorRegionPercept().ballIndexY, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_cb",
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexCb - getBaseColorRegionPercept().distBallCb), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexCb - getBaseColorRegionPercept().distBallCb), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexCb + getBaseColorRegionPercept().distBallCb), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexCb + getBaseColorRegionPercept().distBallCb), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::skyblue,
      Math::clamp((int) getBaseColorRegionPercept().ballIndexCb, 0, 255),
      0,
      Math::clamp((int) getBaseColorRegionPercept().ballIndexCb, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:ball_cr",
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexCr - getBaseColorRegionPercept().distBallCr), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexCr - getBaseColorRegionPercept().distBallCr), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexCr + getBaseColorRegionPercept().distBallCr), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().ballIndexCr + getBaseColorRegionPercept().distBallCr), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::orange,
      Math::clamp((int) getBaseColorRegionPercept().ballIndexCr, 0, 255),
      0,
      Math::clamp((int) getBaseColorRegionPercept().ballIndexCr, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_y",
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexY - getBaseColorRegionPercept().distGoalY), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexY - getBaseColorRegionPercept().distGoalY), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::gray,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexY + getBaseColorRegionPercept().distGoalY), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexY + getBaseColorRegionPercept().distGoalY), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::white,
      Math::clamp((int) getBaseColorRegionPercept().goalIndexY, 0, 255),
      0,
      Math::clamp((int) getBaseColorRegionPercept().goalIndexY, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_cb",
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexCb - getBaseColorRegionPercept().distGoalCb), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexCb - getBaseColorRegionPercept().distGoalCb), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::blue,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexCb + getBaseColorRegionPercept().distGoalCb), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexCb + getBaseColorRegionPercept().distGoalCb), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::skyblue,
      Math::clamp((int) getBaseColorRegionPercept().goalIndexCb, 0, 255),
      0,
      Math::clamp((int) getBaseColorRegionPercept().goalIndexCb, 0, 255),
      imageHeight
    );
    drawRegion = true;
  );

  DEBUG_REQUEST("ImageProcessor:BaseColorClassifier:goal_cr",
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexCr - getBaseColorRegionPercept().distGoalCr), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexCr - getBaseColorRegionPercept().distGoalCr), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::red,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexCr + getBaseColorRegionPercept().distGoalCr), 0, 255),
      0,
      Math::clamp((int) (getBaseColorRegionPercept().goalIndexCr + getBaseColorRegionPercept().distGoalCr), 0, 255),
      imageHeight
    );
    LINE_PX
    (
      ColorClasses::orange,
      Math::clamp((int) getBaseColorRegionPercept().goalIndexCr, 0, 255),
      0,
      Math::clamp((int) getBaseColorRegionPercept().goalIndexCr, 0, 255),
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
