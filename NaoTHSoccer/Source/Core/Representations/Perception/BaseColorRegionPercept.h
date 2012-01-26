#ifndef BASECOLORREGIONPERCEPT_H
#define BASECOLORREGIONPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Representations/Infrastructure/FrameInfo.h>

#define GOAL_IDX_Y 160.0
#define GOAL_IDX_CB 80.0
#define GOAL_IDX_CR 135.0

#define GOAL_DIST_Y 96.0
#define GOAL_DIST_CB 32.0
#define GOAL_DIST_CR 28.0

#define BALL_IDX_Y 140.0
#define BALL_IDX_CB 80.0
#define BALL_IDX_CR 200.0

#define BALL_DIST_Y 140.0
#define BALL_DIST_CB 48.0
#define BALL_DIST_CR 32.0

#define LINE_IDX_Y 254.0
#define LINE_IDX_CB 130.0
#define LINE_IDX_CR 130.0

#define LINE_DIST_Y 72.0
#define LINE_DIST_CB 28.0
#define LINE_DIST_CR 28.0

using namespace naoth;

class BaseColorRegionPercept : public naoth::Printable
{
  class ColorBlob
  {
  private:
    int minY;
    int maxY;
    int minCb;
    int maxCb;
    int minCr;
    int maxCr;
  public:
    ColorBlob(
      int minY,
      int maxY,
      int minCb,
      int maxCb,
      int minCr,
      int maxCr
      ) 
        :
      minY(minY),
      maxY(maxY),
      minCb(minCb),
      maxCb(maxCb),
      minCr(minCr),
      maxCr(maxCr)
    {
    }

    ColorBlob() 
        :
      minY(0),
      maxY(0),
      minCb(0),
      maxCb(0),
      minCr(0),
      maxCr(0)
    {
    }

    inline bool inside(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
    {
      return minY > yy && yy < maxY
          && minCb > cb && cb < maxCb
          && minCr > cr && cr < maxCr;
    }//end inside
  };




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

  ColorBlob goalBlob;


  double ballIndexY;
  double ballIndexCb;
  double ballIndexCr;

  double distBallY;
  double distBallCb;
  double distBallCr;

  ColorBlob ballBlob;


  double lineIndexY;
  double lineIndexCb;
  double lineIndexCr;

  double distLineY;
  double distLineCb;
  double distLineCr;

  ColorBlob lineBlob;


  FrameInfo lastUpdated;

  void cretePercept()
  {
    goalBlob = ColorBlob(
      meanY / 128 * goalIndexY - distGoalY,
      meanY / 128 * goalIndexY + distGoalY,
      meanCb / 128 * goalIndexCb - distGoalCb,
      meanCb / 128 * goalIndexCb + distGoalCb,
      meanCr / 128 * goalIndexCr - distGoalCr,
      meanCr / 128 * goalIndexCr + distGoalCr
      );

    ballBlob = ColorBlob(
      meanY / 128 * ballIndexY - distBallY,
      meanY / 128 * ballIndexY + distBallY,
      meanCb / 128 * ballIndexCb - distBallCb,
      meanCb / 128 * ballIndexCb + distBallCb,
      meanCr / 128 * ballIndexCr - distBallCr,
      meanCr / 128 * ballIndexCr + distBallCr
      );

    lineBlob = ColorBlob(
      meanY / 128 * lineIndexY - distLineY,
      meanY / 128 * lineIndexY + distLineY,
      meanCb / 128 * lineIndexCb - distLineCb,
      meanCb / 128 * lineIndexCb + distLineCb,
      meanCr / 128 * lineIndexCr - distLineCr,
      meanCr / 128 * lineIndexCr + distLineCr
      );
  };


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

    lineIndexY = LINE_IDX_Y;
    lineIndexCb = LINE_IDX_CB;
    lineIndexCr = LINE_IDX_CR;

    distLineY = LINE_DIST_Y;
    distLineCb = LINE_DIST_CB;
    distLineCr = LINE_DIST_CR;

  }

  ~BaseColorRegionPercept()
  {}

  inline bool isYellow(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    return goalBlob.inside(yy,cb,cr);
    /*
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
    */
  }

  inline bool isYellow(const Pixel& pixel) const
  {
    return goalBlob.inside(pixel.y, pixel.u, pixel.v);
    //return isYellow(pixel.y, pixel.u, pixel.v);
  }

  inline bool isRedOrOrangeOrPink(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    return ballBlob.inside(yy,cb,cr);
    /*
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
    );*/
  }

  inline bool isRedOrOrangeOrPink(const Pixel& pixel) const
  {
    ballBlob.inside(pixel.y, pixel.u, pixel.v);
    //return isRedOrOrangeOrPink(pixel.y, pixel.u, pixel.v);
  }

  inline bool isGreenOrBlue(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
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
        yy > 5.0 * meanY / 128
        &&
        yy < 127.0 * meanY / 128
        &&
        cr < 127.0 * meanCr / 128
      )
    );
  }

  inline bool isGreenOrBlue(const Pixel& pixel) const
  {
    return isGreenOrBlue(pixel.y, pixel.u, pixel.v);
  }

  inline bool isGrayLevel(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    double fCb = meanY / 128;//dCb / 127.0;//(127.0 + log(getBlackAndWhitePercept().diffMean));//getColoredGrid().meanBrightness;// 127.0;
    double fCr = meanCr / 128;//dCr / 127.0;//(127.0 + log(getBlackAndWhitePercept().diffMean));//getColoredGrid().meanBrightness;//127.0;
    return
    (
      (
         yy < 5.0
      )
      ||
      (
         yy > 232.0
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

  inline bool isGrayLevel(const Pixel& pixel) const
  {
    return isGrayLevel(pixel.y, pixel.u, pixel.v);
  }

  inline bool isWhite(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    return lineBlob.inside(yy,cb,cr);
    /*
    return
    (
      yy > meanY / 128 * lineIndexY - distLineY
      &&
      yy < meanY / 128 * lineIndexY + distLineY
      &&
      cb > meanCb / 128 * lineIndexCb - distLineCb
      &&
      cb < meanCb / 128 * lineIndexCb + distLineCb
      &&
      cr > meanCr / 128 * lineIndexCr - distLineCr
      &&
      cr < meanCr / 128 * lineIndexCr + distLineCr
    );*/
  }

  inline bool isWhite(const Pixel& pixel) const
  {
    return lineBlob.inside(pixel.y, pixel.u, pixel.v);
    //return isWhite(pixel.y, pixel.u, pixel.v);
  }

  virtual void print(ostream& stream) const
  {
    stream << "dCb: " << dCb << endl;
    stream << "dCr: " << dCr << endl;
    stream << "meanY: " << meanY << endl;
    stream << "meanCb: " << meanCb << endl;
    stream << "meanCr: " << meanCr << endl;

    stream << "ballIndexY: " << ballIndexY << endl;
    stream << "ballIndexCb: " << ballIndexCb << endl;
    stream << "ballIndexCr: " << ballIndexCr << endl;
    stream << "distBallY: " << distBallY << endl;
    stream << "distBallCb: " << distBallCb << endl;
    stream << "distBallCr: " << distBallCr << endl;

    stream << "goalIndexY: " << goalIndexY << endl;
    stream << "goalIndexCb: " << goalIndexCb << endl;
    stream << "goalIndexCr: " << goalIndexCr << endl;
    stream << "distGoalY: " << distGoalY << endl;
    stream << "distGoalCb: " << distGoalCb << endl;
    stream << "distGoalCr: " << distGoalCr << endl;

    stream << "lineIndexY: " << lineIndexY << endl;
    stream << "lineIndexCb: " << lineIndexCb << endl;
    stream << "lineIndexCr: " << lineIndexCr << endl;
    stream << "distLineY: " << distLineY << endl;
    stream << "distLineCb: " << distLineCb << endl;
    stream << "distLineCr: " << distLineCr << endl;
  }//end print


};



#endif // BASECOLORREGIONPERCEPT_H
