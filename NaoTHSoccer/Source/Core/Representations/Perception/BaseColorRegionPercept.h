#ifndef BASECOLORREGIONPERCEPT_H
#define BASECOLORREGIONPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Representations/Infrastructure/FrameInfo.h>

#define GOAL_IDX_Y 160.0
#define GOAL_IDX_CB 80.0
#define GOAL_IDX_CR 150.0

#define GOAL_DIST_Y 96.0
#define GOAL_DIST_CB 32.0
#define GOAL_DIST_CR 24.0

#define BALL_IDX_Y 140.0
#define BALL_IDX_CB 80.0
#define BALL_IDX_CR 200.0

#define BALL_DIST_Y 140.0
#define BALL_DIST_CB 42.0
#define BALL_DIST_CR 56.0



using namespace naoth;

class BaseColorRegionPercept : public naoth::Printable
{
private:
  double dCb;
  double dCr;

public:
  double meanY;
  double meanCr;
  double meanCb;
  double diffY;
  double diffCr;
  double diffCb;

  double distY;
  double distCb;
  double distCr;

  double goalIndexY;
  double goalIndexCb;
  double goalIndexCr;

  double distGoalY;
  double distGoalCb;
  double distGoalCr;

  double ballIndexY;
  double ballIndexCb;
  double ballIndexCr;

  double distBallY;
  double distBallCb;
  double distBallCr;

  FrameInfo lastUpdated;

  BaseColorRegionPercept()
  {
    dCb = 96.0;
    dCr = 48.0;

    meanY = 127.0;
    meanCr = 127.0;
    meanCb = 127.0;

    diffY = 0.0;
    diffCr = 0.0;
    diffCb = 0.0;

    goalIndexY = GOAL_IDX_Y;
    goalIndexCb = GOAL_IDX_CB;
    goalIndexCr = GOAL_IDX_CR;

    distGoalY = GOAL_DIST_Y;
    distGoalCb = GOAL_DIST_CB;
    distGoalCr = GOAL_DIST_CR;

    ballIndexY = BALL_IDX_Y;
    ballIndexCb = BALL_IDX_CB;
    ballIndexCr = BALL_IDX_CR;

    distBallY = BALL_DIST_Y;
    distBallCb = BALL_DIST_CB;
    distBallCr = BALL_DIST_CR;
   }

  ~BaseColorRegionPercept()
  {}

  bool isYellow(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    return
    (
      yy > meanY / 128 * goalIndexY - distGoalY
      &&
      yy < meanY / 128 * goalIndexY + distGoalY
      &&
      cb > meanCb / 128 * goalIndexCb - distGoalCb
      &&
      cb < meanCb / 128 * goalIndexCb + distGoalCb
      &&
      cr > meanCr / 128 * goalIndexCr - distGoalCr
      &&
      cr < meanCr / 128 * goalIndexCr + distGoalCr
    );
  }

  bool isYellow(const Pixel& pixel) const
  {
    return isYellow(pixel.y, pixel.u, pixel.v);
  }

  bool isRedOrOrangeOrPink(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {

    return
    (
      yy > meanY / 128 * ballIndexY - distBallY
      &&
      yy < meanY / 128 * ballIndexY + distBallY
      &&
      cb > meanCb / 128 * ballIndexCb - distBallCb
      &&
      cb < meanCb / 128 * ballIndexCb + distBallCb
      &&
      cr > meanCr / 128 * ballIndexCr - distBallCr
      &&
      cr < meanCr / 128 * ballIndexCr + distBallCr
    );
  }

  bool isRedOrOrangeOrPink(const Pixel& pixel) const
  {
    return isRedOrOrangeOrPink(pixel.y, pixel.u, pixel.v);
  }

  bool isGreenOrBlue(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    return
    (
      (
        yy < 240.0 * meanY / 128
        &&
        yy >= 127.0 * meanY / 128
        &&
        cr < 127.0 * meanCr / 128
      )
      ||
      (
        yy > 8.0 * meanY / 128
        &&
        yy < 127.0 * meanY / 128
        &&
        cr < 127.0 * meanCr / 128
      )
    );
  }

  bool isGreenOrBlue(const Pixel& pixel) const
  {
    return isGreenOrBlue(pixel.y, pixel.u, pixel.v);
  }

  bool isGrayLevel(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    double fCb = dCb / 127.0;//(127.0 + log(getBlackAndWhitePercept().diffMean));//getColoredGrid().meanBrightness;// 127.0;
    double fCr = dCr / 127.0;//(127.0 + log(getBlackAndWhitePercept().diffMean));//getColoredGrid().meanBrightness;//127.0;
    return
    (
      (
         yy < 20.0
      )
      ||
      (
         yy > 224.0
      )
      ||
      (
        yy > 127.0
        &&
        cb > 127.0
        &&
        cr > 127.0
        &&
        cb < (127.0 + yy * fCb)
        &&
        cb < (127.0 + cr * fCr)
        &&
        cr < (127.0 + yy * fCr)
        &&
        cr < (127.0 + cb * fCb)
      )
      ||
      (
        yy < 127.0
        &&
        cb < 127.0
        &&
        cr < 127.0
        &&
        cb > (127.0 - yy * fCb)
        &&
        cb > (127.0 - cr * fCr)
        &&
        cr > (127.0 - yy * fCr)
        &&
        cr > (127.0 - cb * fCb)
      )
    );
  }

  bool isGrayLevel(const Pixel& pixel) const
  {
    return isGrayLevel(pixel.y, pixel.u, pixel.v);
  }

  virtual void print(ostream& stream) const
  {
    stream << "dCb: " << dCb << endl;
    stream << "dCr: " << dCr << endl;
    stream << "meanY: " << meanY << endl;
    stream << "meanCb: " << meanCb << endl;
    stream << "meanCr: " << meanCr << endl;
    stream << "goalIndexY: " << goalIndexY << endl;
    stream << "goalIndexCb: " << goalIndexCb << endl;
    stream << "goalIndexCr: " << goalIndexCr << endl;
    stream << "distGoalY: " << distGoalY << endl;
    stream << "distGoalCb: " << distGoalCb << endl;
    stream << "distGoalCr: " << distGoalCr << endl;
  }//end print

};



#endif // BASECOLORREGIONPERCEPT_H
