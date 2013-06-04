#ifndef BASECOLORREGIONPERCEPT_H
#define BASECOLORREGIONPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Tools/ImageProcessing/ImagePrimitives.h>
//#include <Tools/ImageProcessing/BaseColorRegionParameters.h>
#include "Tools/ImageProcessing/ColorRegion.h"

#include <Representations/Infrastructure/FrameInfo.h>

#include "math.h"

using namespace naoth;

class BaseColorRegionPercept : public naoth::Printable
{
  class BaseColorRegion
  {
  private:
    DoublePixel min;
    DoublePixel max;
    bool valid;
  public:
    BaseColorRegion()
    {
      set(0, 0, 0, 0, 0, 0);
      valid = false;
    }

    void set
    (
      double minY,
      double maxY,
      double minU,
      double maxU,
      double minV,
      double maxV
    )
    {
      set
      (
        (int) floor(minY),
        (int) ceil(maxY),
        (int) floor(minU),
        (int) ceil(maxU),
        (int) floor(minV),
        (int) ceil(maxV)
      );
    }

    void set
    (
      int minY,
      int maxY,
      int minU,
      int maxU,
      int minV,
      int maxV
    )
    {
      min.y = minY;
      min.u = minU;
      min.v = minV;
      max.y = maxY;
      max.u = maxU;
      max.v = maxV;
    }

    void set
    (
        DoublePixel& min_,
        DoublePixel& max_
    )
    {
      min = min_;
      max = max_;
    }


    inline bool inside(const int& y, const int& u, const int& v) const
    {
      return min.y <= y && y <= max.y && min.u <= u && u <= max.u && min.v <= v && v <= max.v;
    }//end inside
  };

public:

  PixelT<double> distGray;
  PixelT<double> grayLevel;

  bool ballColorIsCalibrated;
  bool goallColorIsCalibrated;
  bool linesColorIsCalibrated;
  bool fieldColorIsCalibrated;

  PixelT<double> ballImageMean;
  PixelT<double> goalImageMean;
  PixelT<double> linesImageMean;
  PixelT<double> fieldImageMean;

  PixelT<double> meanEnv;
  PixelT<double> meanImg;

  PixelT<double> diff;

  PixelT<double> dist;

  PixelT<double> fieldIndex;
  PixelT<double> fieldCalibIndex;
  PixelT<double> fieldDist;
  PixelT<double> fieldCalibDist;

  double goalVUdistance;
  double goalVUdistanceMin;
  double goalVUdistanceMax;
  
  PixelT<double> goalIndex;
  PixelT<double> goalCalibIndex;
  PixelT<double> goalDist;
  PixelT<double> goalCalibDist;

  PixelT<double> ballIndex;
  PixelT<double> ballCalibIndex;
  PixelT<double> ballDist;
  PixelT<double> ballCalibDist;

  PixelT<double> lineIndex;
  PixelT<double> lineCalibIndex;
  PixelT<double> lineDist;
  PixelT<double> lineCalibDist;

  PixelT<double> fieldBorderPlus;
  PixelT<double> fieldBorderMinus;

  PixelT<double> goalBorderPlus;
  PixelT<double> goalBorderMinus;

  PixelT<double> ballBorderPlus;
  PixelT<double> ballBorderMinus;

  PixelT<double> lineBorderPlus;
  PixelT<double> lineBorderMinus;

  BaseColorRegion fieldColorRegion;
  BaseColorRegion goalColorRegion;
  BaseColorRegion ballColorRegion;
  BaseColorRegion lineColorRegion;

  yellowColorRegion yellowGoal;
  skyblueColorRegion blueGoal;
  orangeColorRegion orangeBall;
  pinkColorRegion pinkWaistBand;
  blueColorRegion blueWaistBand;
  whiteColorRegion whiteLine;

  FrameInfo lastUpdated;

  BaseColorRegionPercept()
  {
    distGray.y = 255.0;
    distGray.u = 96.0;
    distGray.v = 48.0;

    goalVUdistance = 0.0;
    goalVUdistanceMin = 0.0;
    goalVUdistanceMax = 0.0;

    meanImg.y = 127.0;
    meanImg.u = 127.0;
    meanImg.v = 127.0;

    meanEnv = meanImg;

    fieldImageMean = meanImg;
    goalImageMean = meanImg;
    ballImageMean = meanImg;
    linesImageMean = meanImg;

    diff.y = 0.0;
    diff.u = 0.0;
    diff.v = 0.0;
  }

  ~BaseColorRegionPercept()
  {}

  inline void setPerceptRegions()
  {
    double diffMeanY = 0.0;//127.5 - meanEnv.y;
    double diffMeanU = 0.0;//127.5 - meanEnv.u;
    double diffMeanV = 0.0;//127.5 - meanEnv.v;

    double fY = meanImg.y / fieldImageMean.y;
    double fdY = fY * 0.5;//1;//fabs(fieldImageMean.y - meanImg.y) / meanEnv.y;
    double dPlusY = fY * (fieldIndex.y - diffMeanY) + fdY * fieldDist.y;
    double dMinusY = fY * (fieldIndex.y- diffMeanY) - fdY * fieldDist.y;

    double fU = meanImg.u / fieldImageMean.u;
    double fdU = 1;//fabs(fieldImageMean.u - meanImg.u) / meanEnv.u;
    double dPlusU = fU * (fieldIndex.u - diffMeanU + (fieldCalibIndex.y - meanImg.y) / 5) + fdU * fieldDist.u;
    double dMinusU = fU * (fieldIndex.u - diffMeanU + (fieldCalibIndex.y - meanImg.y) / 5) - fdU * fieldDist.u;

    double fV = meanImg.v / fieldImageMean.v;
    double fdV = 1;//fabs(fieldImageMean.v - meanImg.v) / meanEnv.v;
    double dPlusV = fV * (fieldIndex.v + diffMeanV + (fieldCalibIndex.y - meanImg.y) / 5) + fdV * fieldDist.v;
    double dMinusV = fV * (fieldIndex.v + diffMeanV + (fieldCalibIndex.y - meanImg.y) / 5) - fdV * fieldDist.v;

    fieldBorderPlus.y = dPlusY;
    fieldBorderMinus.y = dMinusY;
    fieldBorderPlus.u = dPlusU;
    fieldBorderMinus.u = dMinusU;
    fieldBorderPlus.v = dPlusV;
    fieldBorderMinus.v = dMinusV;
    fieldColorRegion.set(dMinusY, dPlusY, dMinusU, dPlusU, dMinusV, dPlusV);

    fY = 1;//meanImg.y / goalImageMean.y;
    fdY = 1;//fabs(goalImageMean.y - meanEnv.y) / meanImg.y;
//    dPlusY = fY * goalIndex.y + fdY * goalDist.y;
//    dMinusY = fY * goalIndex.y - fdY * goalDist.y;
    dPlusY = fY * (goalIndex.y) + fdY * goalDist.y;
    dMinusY = fY * (goalIndex.y ) - fdY * goalDist.y;

    fU = 1;//meanImg.u / goalImageMean.u;
    fdU = 1;//fabs(goalImageMean.u - meanEnv.u) / meanImg.u;
//    dPlusU = fU * (goalIndex.u + (goalCalibIndex.y - meanImg.y) / 5) + fdU * goalDist.u;
//    dMinusU = fU * (goalIndex.u + (goalCalibIndex.y - meanImg.y) / 5) - fdU * goalDist.u;
    dPlusU = fU * (goalIndex.u ) + fdU * goalDist.u;
    dMinusU = fU * (goalIndex.u  ) - fdU * goalDist.u;
//    dPlusU = fU * (goalIndex.u - diffMeanU + (goalCalibIndex.y - meanImg.y) / 5) + fdU * goalDist.u;
//    dMinusU = fU * (goalIndex.u - diffMeanU + (goalCalibIndex.y - meanImg.y) / 5) - fdU * goalDist.u;

    fV = 1;//meanImg.v / goalImageMean.v;
    fdV = 1;//fabs(goalImageMean.v - meanEnv.v) / meanImg.v;
//    dPlusV = fV * (goalIndex.v - (goalCalibIndex.y - meanImg.y) / 5) + fdV * goalDist.v;
//    dMinusV = fV * (goalIndex.v - (goalCalibIndex.y - meanImg.y) / 5) - fdV * goalDist.v;
    dPlusV = fV * (goalIndex.v  ) + fdV * goalDist.v;
    dMinusV = fV * (goalIndex.v  ) - fdV * goalDist.v;
//    dPlusV = fV * (goalIndex.v - diffMeanV - (goalCalibIndex.y - meanImg.y) / 5) + fdV * goalDist.v;
//    dMinusV = fV * (goalIndex.v - diffMeanV - (goalCalibIndex.y - meanImg.y) / 5) - fdV * goalDist.v;

    goalBorderPlus.y = dPlusY;
    goalBorderMinus.y = dMinusY;
    goalBorderPlus.u = dPlusU;
    goalBorderMinus.u = dMinusU;
    goalBorderPlus.v = dPlusV;
    goalBorderMinus.v = dMinusV;
    goalColorRegion.set(goalBorderMinus, goalBorderPlus);

    fY = meanImg.y / ballImageMean.y;
    dPlusY = fY * ballIndex.y + ballDist.y;
    dMinusY = fY * ballIndex.y - ballDist.y;

    fU = meanImg.u / ballImageMean.u;
    dPlusU = fU * ballIndex.u + ballDist.u;
    dMinusU = fU * ballIndex.u - ballDist.u;

    fV = meanImg.v / ballImageMean.v;
    dPlusV = fV * ballIndex.v + ballDist.v;
    dMinusV = fV * ballIndex.v - ballDist.v;

    ballBorderPlus.y = dPlusY;
    ballBorderMinus.y = dMinusY;
    ballBorderPlus.u = dPlusU;
    ballBorderMinus.u = dMinusU;
    ballBorderPlus.v = dPlusV;
    ballBorderMinus.v = dMinusV;
    ballColorRegion.set(ballBorderMinus, ballBorderPlus);

    fY = 1;//meanImg.y / linesImageMean.y;
    dPlusY = fY * lineIndex.y + lineDist.y;
    dMinusY = fY * lineIndex.y - lineDist.y;

    fU = 1;//meanImg.u / linesImageMean.u;
    dPlusU = fU * lineIndex.u + lineDist.u;
    dMinusU = fU * lineIndex.u - lineDist.u;

    fV = 1;//meanImg.v / linesImageMean.v;
    dPlusV = fV * lineIndex.v + lineDist.v;
    dMinusV = fV * lineIndex.v - lineDist.v;

    lineBorderPlus.y = dPlusY;
    lineBorderMinus.y = dMinusY;
    lineBorderPlus.u = dPlusU;
    lineBorderMinus.u = dMinusU;
    lineBorderPlus.v = dPlusV;
    lineBorderMinus.v = dMinusV;
    lineColorRegion.set(lineBorderMinus, lineBorderPlus);


    //dY = meanImg.y / 127.0;//dCb / 127.0;//(127.0 + log(getBlackAndWhitePercept().diffMean));//getColoredGrid().meanBrightness;// 127.0;
    //dCb = meanImg.u / 127.0;//dCb / 127.0;//(127.0 + log(getBlackAndWhitePercept().diffMean));//getColoredGrid().meanBrightness;// 127.0;
    //dCr = meanImg.v / 127.0;//dCr / 127.0;//(127.0 + log(getBlackAndWhitePercept().diffMean));//getColoredGrid().meanBrightness;//127.0;
  }

  inline bool isYellow(const int& y, const int& u, const int& v) const
  {
    return goalColorRegion.inside(y, u, v) && v - u > goalVUdistanceMin && v - u < goalVUdistanceMax;
  }

  inline bool isYellowSimple(const Pixel& pixel) const
  {
    return pixel.v - pixel.u > goalVUdistance;
  }

  inline bool isPlainYellow(const Pixel& pixel) const
  {
    return goalColorRegion.inside(pixel.y, pixel.u, pixel.v);
  }

  inline bool isYellow(const Pixel& pixel) const
  {
    return isYellow((int) pixel.y, (int) pixel.u, (int) pixel.v);
  }

  inline bool isRedOrOrangeOrPink(const int& y, const int& u, const int& v) const
  {
    return/* !isYellow(y, u, v) &&*/ ballColorRegion.inside(y, u, v);
  }

  inline bool isRedOrOrangeOrPink(const Pixel& pixel) const
  {
    return isRedOrOrangeOrPink((int) pixel.y, (int) pixel.u, (int) pixel.v);
  }

  inline bool isGreenOrBlue(const int& y, const int& u, const int& v) const
  {
    return
    (
//      y < meanEnv.y
//      &&
//      y < meanImg.y
//      &&
      fieldColorRegion.inside(y, u, v)
//      (
//        y < FieldMaxY1
//        &&
//        y >= FieldMinY1
//        &&
//        v < FieldMaxV
//      )
//      ||
//      (
//        y > FieldMinY2
//        &&
//        y < FieldMaxY2
//        &&
//        v < FieldMaxV
//        y < 240.0 * meanImg.y / 128
//        &&
//        y >= 127.0 * meanImg.y / 128
//        &&
//        v < 127.0 * meanImg.v / 128
//      )
//      ||
//      (
//        y > 5.0 * meanImg.y / 128
//        &&
//        y < 127.0 * meanImg.y / 128
//        &&
//        v < 127.0 * meanImg.v / 128
//      )
    );
  }

  inline bool isGreenOrBlue(const Pixel& pixel) const
  {
    return isGreenOrBlue((int) pixel.y, (int) pixel.u, (int) pixel.v);
  }

  inline bool isGrayLevel(const int& y, const int& u, const int& v) const
  {
    return
    (
      (
        y > meanEnv.y// grayLevel.y
        &&
        u > grayLevel.u
        &&
        v > grayLevel.v
        &&
        u < (127.0 + y * distGray.u)
        &&
        u < (127.0 + v * distGray.v)
        &&
        v < (127.0 + y * distGray.v)
        &&
        v < (127.0 + u * distGray.u)
      )
      ||
      (
        y <= meanEnv.y//grayLevel.y
        &&
        u <= grayLevel.u
        &&
        v <= grayLevel.v
        &&
        u > (127.0 - y * distGray.u)
        &&
        u > (127.0 - v * distGray.v)
        &&
        v > (127.0 - y * distGray.v)
        &&
        v > (127.0 - u * distGray.u)
      )
    );
  }

  inline bool isGrayLevel(const Pixel& pixel) const
  {
    return isGrayLevel(pixel.y, pixel.u, pixel.v);
  }

  inline bool isWhiteColorModel(const int& y, const int& u, const int& v) const
  {
    return (isGrayLevel(y, u, v) || lineColorRegion.inside(y, u, v));
  }

  inline bool isWhite(const int& y, const int& u, const int& v) const
  {
    return (isGrayLevel(y, u, v) || lineColorRegion.inside(y, u, v)) && !isRedOrOrangeOrPink(y, u, v) && !isYellow(y, u, v);
  }

  inline bool isWhite(const Pixel& pixel) const
  {
    return isWhite(pixel.y, pixel.u, pixel.v);
  }

  inline void print(std::ostream& stream) const
  {
    stream << "meanEnv.y: " << meanEnv.y << std::endl;
    stream << "meanEnv.u: " << meanEnv.u << std::endl;
    stream << "meanEnv.v: " << meanEnv.v << std::endl;
    stream << "meanImg.y: " << meanImg.y << std::endl;
    stream << "meanImg.u: " << meanImg.u << std::endl;
    stream << "meanImg.v: " << meanImg.v << std::endl;

    stream << "fieldIndexY: " << fieldIndex.y << std::endl;
    stream << "fieldIndexU: " << fieldIndex.u << std::endl;
    stream << "fieldIndexV: " << fieldIndex.v << std::endl;
    stream << "fieldDistY: " << fieldDist.y << std::endl;
    stream << "fieldDistU: " << fieldDist.u << std::endl;
    stream << "fieldDistV: " << fieldDist.v << std::endl;
    
    stream << "goalVUdistanceMin: " << goalVUdistanceMin << std::endl;
    stream << "goalVUdistanceMax: " << goalVUdistanceMax << std::endl;

    stream << "goalIndexY: " << goalIndex.y << std::endl;
    stream << "goalIndexU: " << goalIndex.u << std::endl;
    stream << "goalIndexV: " << goalIndex.v << std::endl;
    stream << "goalDistY: " << goalDist.y << std::endl;
    stream << "goalDistU: " << goalDist.u << std::endl;
    stream << "goalDistV: " << goalDist.v << std::endl;

    stream << "ballIndexY: " << ballIndex.y << std::endl;
    stream << "ballIndexU: " << ballIndex.u << std::endl;
    stream << "ballIndexV: " << ballIndex.v << std::endl;
    stream << "ballDistY: " << ballDist.y << std::endl;
    stream << "ballDistU: " << ballDist.u << std::endl;
    stream << "ballDistV: " << ballDist.v << std::endl;

    stream << "lineIndexY: " << lineIndex.y << std::endl;
    stream << "lineIndexU: " << lineIndex.u << std::endl;
    stream << "lineIndexV: " << lineIndex.v << std::endl;
    stream << "lineDistY: " << lineDist.y << std::endl;
    stream << "lineDistU: " << lineDist.u << std::endl;
    stream << "lineDistV: " << lineDist.v << std::endl;
  }//end print


};

class BaseColorRegionPerceptTop : public BaseColorRegionPercept
{
public:
  virtual ~BaseColorRegionPerceptTop() {}
};


#endif // BASECOLORREGIONPERCEPT_H
