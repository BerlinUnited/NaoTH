/* 
 * File:   FieldColorPercept.h
 * Author: claas
 *
 * Created on 17. march 2011, 22:53
 */

#ifndef FIELDCOLORPERCEPT_H
#define FIELDCOLORPERCEPT_H

#include <algorithm>
#include <Tools/Math/Common.h>
#include <Tools/Math/Vector2.h>

#include <Tools/DataStructures/Printable.h>
#include <Representations/Infrastructure/FrameInfo.h>

using namespace naoth;

class FieldColorPercept : public naoth::Printable
{

  class ColorRange
  {
  protected:
    PixelT<int> min;
    PixelT<int> max;

  public:
    ColorRange(){}

    void set(int minY, int minU, int minV, int maxY, int maxU, int maxV)
    {
      min.y = minY;
      min.u = minU;
      min.v = minV;
      max.y = maxY;
      max.u = maxU;
      max.v = maxV;
    }

    PixelT<int> getMax() const {
      return max;
    }

    PixelT<int> getMin() const {
      return min;
    }

    inline bool inside(const Pixel& pixel) const
    {
      return  min.y <= pixel.y && pixel.y <= max.y && 
              min.u <= pixel.u && pixel.u <= max.u && 
              min.v <= pixel.v && pixel.v <= max.v;
    }

    inline bool inside(int y, int u, int v) const
    {
      return  min.y <= y && y <= max.y && 
              min.u <= u && u <= max.u && 
              min.v <= v && v <= max.v;
    }
  };

public:
  class HSISeparatorOptimized
  {
  public:

    class Parameter {
    public:
      Parameter() 
        :
        brightnesConeOffset(0),
        brightnesConeRadiusWhite(0),
        brightnesConeRadiusBlack(0),
        colorAngleCenter(0),
        colorAngleWith(0)
      {}
      int brightnesConeOffset;
      double brightnesConeRadiusWhite;
      double brightnesConeRadiusBlack;

      double colorAngleCenter; 
      double colorAngleWith;
    };

  public:

    HSISeparatorOptimized()
      : distMin(0), distMax(0)
    {
      // initialize the array
      for(size_t i=0; i < 256; i++)
      {
        brightnessThreshold[i] = 0;
      }
    }

    void set(const Parameter& p) {
      setColor(p.colorAngleCenter, p.colorAngleWith);
      setBrightness(p.brightnesConeOffset, p.brightnesConeRadiusBlack, p.brightnesConeRadiusWhite);
    }

    void setColor(double colorAngleCenter, double colorAngleWith) {
      normalMin = Vector2i(1024,0);
      normalMin.rotate(colorAngleCenter - colorAngleWith + Math::pi_2);

      distMin = normalMin*Vector2i(128,128);

      normalMax = Vector2i(1024,0);
      normalMax.rotate(colorAngleCenter + colorAngleWith + Math::pi_2);

      distMax = normalMax*Vector2i(128,128);

      p.colorAngleCenter = colorAngleCenter;
      p.colorAngleWith = colorAngleWith;
    }

    void setBrightness(int brightnesConeOffset, double brightnesConeRadiusBlack, double brightnesConeRadiusWhite) {
      this->brightnesAlpha = (brightnesConeRadiusWhite - brightnesConeRadiusBlack) / (double)(255 - brightnesConeOffset);
      
      for(int i = 0; i < 256; i++) {
        double t = std::max(brightnesConeRadiusBlack, brightnesConeRadiusBlack + brightnesAlpha * (double)(i-brightnesConeOffset));
        brightnessThreshold[i] = (int)(t*t + 0.5);
      }

      p.brightnesConeOffset = brightnesConeOffset;
      p.brightnesConeRadiusWhite = brightnesConeRadiusWhite;
      p.brightnesConeRadiusBlack = brightnesConeRadiusBlack;
    }

    inline bool isColor(int y, int u, int v) const {
      return !noColor(y,u,v) && isChroma(y,u,v);
    }

    inline bool isColor(const Pixel& pixel) const {
      return isColor(pixel.y, pixel.u, pixel.v);
    }

    inline bool isChroma(const Pixel& pixel) const {
      return isChroma(pixel.y, pixel.u, pixel.v);
    }

    inline bool isChroma(int /*y*/, int u, int v) const {
      return normalMin.x*u + normalMin.y*v > distMin && 
             normalMax.x*u + normalMax.y*v < distMax;
    }

    inline bool noColor(int y, int u, int v) const {
      //double cromaThreshold = std::max(brightnesConeRadiusBlack, brightnesConeRadiusBlack + brightnesAlpha * (double)(y-brightnesConeOffset));
      return Math::sqr(u - 128) + Math::sqr(v - 128) < brightnessThreshold[y]; //Vector2d(u - 128, v - 128).abs() < brightnessThreshold[y];
    }

    inline bool noColor(const Pixel& pixel) const {
      return noColor(pixel.y, pixel.u, pixel.v);
    }

    const Parameter& getParams() const {
      return p;
    }

    private:
      Parameter p;
      double brightnesAlpha; // calculated

      //double colorAngleCenter;
      //double colorAngleWith;
      Vector2i normalMin;
      int distMin;
      Vector2i normalMax;
      int distMax;

      int brightnessThreshold[256];
  };

  class HSISeparator
  {
  public:
    int brightnesConeOffset;
    double brightnesConeRadiusWhite;
    double brightnesConeRadiusBlack;

    double colorAngleCenter;
    double colorAngleWith;

    inline bool isColor(int y, int u, int v) const {
      
      const double brightnesAlpha = (brightnesConeRadiusWhite - brightnesConeRadiusBlack) / (double)(255 - brightnesConeOffset);

      Vector2d dp(u - 128, v - 128);
      double d = dp.abs();
      double a = dp.angle();

      double cromaThreshold = std::max(brightnesConeRadiusBlack, brightnesConeRadiusBlack + brightnesAlpha * (double)(y-brightnesConeOffset));
      return d > cromaThreshold && fabs(Math::normalize(colorAngleCenter - a)) < colorAngleWith;
    }

    inline bool noColor(int y, int u, int v) const {
      const double brightnesAlpha = (brightnesConeRadiusWhite - brightnesConeRadiusBlack) / (double)(255 - brightnesConeOffset);
      double cromaThreshold = std::max(brightnesConeRadiusBlack, brightnesConeRadiusBlack + brightnesAlpha * (double)(y-brightnesConeOffset));
      return Vector2d(u - 128, v - 128).abs2() < cromaThreshold*cromaThreshold;
    }
  };

public:

  HSISeparatorOptimized greenHSISeparator;
  HSISeparatorOptimized redHSISeparator;

  FrameInfo lastUpdated;
  ColorRange range;
  Pixel histogramField;


  FieldColorPercept(){}
  ~FieldColorPercept(){}


  inline bool isFieldColor(int y, int u, int v) const {
    return greenHSISeparator.isColor(y,u,v);
  }

  inline bool isFieldColor(const Pixel& pixel) const {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }

  inline bool isRedColor(int y, int u, int v) const {
    return redHSISeparator.isColor(y,u,v);
  }

  inline bool isRedColor(const Pixel& pixel) const {
    return isRedColor(pixel.y, pixel.u, pixel.v);
  }



  inline void print(std::ostream& stream) const
  {
    static_cast<void>(stream);
  }

};

class FieldColorPerceptTop : public FieldColorPercept
{
public:
  virtual ~FieldColorPerceptTop() {}
};


#endif  /* FIELDCOLORPERCEPT_H */

