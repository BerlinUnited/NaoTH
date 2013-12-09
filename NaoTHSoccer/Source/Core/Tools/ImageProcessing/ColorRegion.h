#ifndef COLORREGION_H
#define COLORREGION_H

#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/Debug/DebugParameterList.h>
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Vector3.h>

using namespace naoth;

template <typename T>
union colorPixelT
{
  struct
  {
    /** Brightness */
    T Y;
    /** U, also known as Cb */
    T U;
    /** V, also known as Cr */
    T V;
  };

  struct
  {
    T Yy;
    T Cb;
    T Cr;
  };

  struct
  {
    T VminusU;
    T UminusY;
    T VminusY;
  };

  T channels[3];
};

typedef colorPixelT<int> colorPixel;

class ColorSpace
{
protected:
  colorPixel colorChannelDistance;
  colorPixel colorChannelIndex;
  colorPixel chLow;
  colorPixel chHigh;

public:

  ColorSpace()
  {
    colorChannelDistance.Y = 0;
    colorChannelDistance.U = 0;
    colorChannelDistance.V = 0;
    colorChannelIndex.Y = -1;
    colorChannelIndex.U = -1;
    colorChannelIndex.V = -1;
    chLow.Y = -1;
    chLow.U = -1;
    chLow.V = -1;
    chHigh.Y = -1;
    chHigh.U = -1;
    chHigh.V = -1;
  }

  void set(PixelT<int> idx, PixelT<int> dist)
  {
    colorChannelIndex.Y = idx.y;
    colorChannelIndex.U = idx.u;
    colorChannelIndex.V = idx.v;
    colorChannelDistance.Y = dist.y;
    colorChannelDistance.U = dist.u;
    colorChannelDistance.V = dist.v;
    sync();
  }

  void get(PixelT<int>& idx, PixelT<int>& dist)
  {
    idx.y = colorChannelIndex.Y;
    idx.u = colorChannelIndex.U;
    idx.v = colorChannelIndex.V;

    dist.y = colorChannelDistance.Y;
    dist.u = colorChannelDistance.U;
    dist.v = colorChannelDistance.V;
    sync();
  }

  void sync()
  {
    chLow.Y = Math::clamp<int>(colorChannelIndex.Y - colorChannelDistance.Y, 0, 255);
    chLow.U = Math::clamp<int>(colorChannelIndex.U - colorChannelDistance.U, 0, 255);
    chLow.V = Math::clamp<int>(colorChannelIndex.V - colorChannelDistance.V, 0, 255);

    chHigh.Y = Math::clamp<int>(colorChannelIndex.Y + colorChannelDistance.Y, 0, 255);
    chHigh.U = Math::clamp<int>(colorChannelIndex.U + colorChannelDistance.U, 0, 255);
    chHigh.V = Math::clamp<int>(colorChannelIndex.V + colorChannelDistance.V, 0, 255);
  }

  inline bool isInColorSpace(const Pixel& pixel)
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
 };

class ColorDifferenceSpace
{
protected:
  colorPixel colorDifferenceDistance;
  colorPixel colorDifferenceIndex;
  colorPixel cDiffLow;
  colorPixel cDiffHigh;

public:
  ColorDifferenceSpace()
  {
    colorDifferenceDistance.Y = 0;
    colorDifferenceDistance.U = 0;
    colorDifferenceDistance.V = 0;
    colorDifferenceIndex.Y = -1;
    colorDifferenceIndex.U = -1;
    colorDifferenceIndex.V = -1;
    cDiffLow.Y = -1;
    cDiffLow.U = -1;
    cDiffLow.V = -1;
    cDiffHigh.Y = -1;
    cDiffHigh.U = -1;
    cDiffHigh.V = -1;
 }

  void sync()
  {    
    cDiffLow.VminusU = Math::clamp<int>(colorDifferenceIndex.VminusU - colorDifferenceDistance.VminusU, -255, 255);
    cDiffLow.UminusY = Math::clamp<int>(colorDifferenceIndex.UminusY - colorDifferenceDistance.UminusY, -255, 255);
    cDiffLow.VminusY = Math::clamp<int>(colorDifferenceIndex.VminusY - colorDifferenceDistance.VminusY, -255, 255);
    cDiffHigh.VminusU = Math::clamp<int>(colorDifferenceIndex.VminusU + colorDifferenceDistance.VminusU, -255, 255);
    cDiffHigh.UminusY = Math::clamp<int>(colorDifferenceIndex.UminusY + colorDifferenceDistance.UminusY, -255, 255);
    cDiffHigh.VminusY = Math::clamp<int>(colorDifferenceIndex.VminusY + colorDifferenceDistance.VminusY, -255, 255);      
  }

  void set(colorPixel idx, colorPixel dist)
  {
    colorDifferenceIndex = idx;
    colorDifferenceDistance = dist;
    sync();
  }

  void get(colorPixel& idx, colorPixel& dist)
  {
    idx = colorDifferenceIndex;
    dist = colorDifferenceDistance;
    sync();
  }

  inline bool isInColorDifferenceSpace(const Pixel& pixel)
  {
    return cDiffLow.Y <= pixel.y && pixel.y <= cDiffHigh.Y && 
            cDiffLow.U <= pixel.u && pixel.u <= cDiffHigh.U && 
            cDiffLow.V <= pixel.v && pixel.v <= cDiffHigh.V;
  }
};

class ColorRegion : public ColorSpace, public ColorDifferenceSpace
{
public:
  ColorClasses::Color color;
 
  ColorRegion(ColorClasses::Color regionColor)
  :
    color(regionColor)
  {}

  ~ColorRegion()
  {}

  void sync()
  {
    ColorSpace::sync();
    ColorDifferenceSpace::sync();
  }
  
  void set(PixelT<int> idx, PixelT<int> dist)
  {
    ColorSpace::set(idx, dist);
  }
  
  void set(colorPixel idx, colorPixel dist)
  {
    ColorDifferenceSpace::set(idx, dist);
  }

  void get(PixelT<int>& idx, PixelT<int>& dist)
  {
    ColorSpace::get(idx, dist);
  }
  
  void get(colorPixel& idx, colorPixel& dist)
  {
    ColorDifferenceSpace::get(idx, dist);
  }

};

class ColorRegionParameter : public ColorRegion, public ParameterList
{
public:

  ColorRegionParameter(std::string regionName, ColorClasses::Color regionColor)
  : 
    ColorRegion(color),
    ParameterList("ColorRegion_" + regionName)
  {
    PARAMETER_REGISTER(colorChannelDistance.Y) = colorChannelDistance.Y;        
    PARAMETER_REGISTER(colorChannelDistance.U) = colorChannelDistance.U;        
    PARAMETER_REGISTER(colorChannelDistance.V) = colorChannelDistance.V;        
    PARAMETER_REGISTER(colorChannelIndex.Y) = colorChannelIndex.Y;        
    PARAMETER_REGISTER(colorChannelIndex.U) = colorChannelIndex.U;        
    PARAMETER_REGISTER(colorChannelIndex.V) = colorChannelIndex.V;        
    PARAMETER_REGISTER(colorDifferenceDistance.VminusU) = colorDifferenceDistance.VminusU;        
    PARAMETER_REGISTER(colorDifferenceDistance.UminusY) = colorDifferenceDistance.UminusY;        
    PARAMETER_REGISTER(colorDifferenceDistance.VminusY) = colorDifferenceDistance.VminusY;        
    PARAMETER_REGISTER(colorDifferenceIndex.VminusU) = colorDifferenceIndex.VminusU;        
    PARAMETER_REGISTER(colorDifferenceIndex.UminusY) = colorDifferenceIndex.UminusY;        
    PARAMETER_REGISTER(colorDifferenceIndex.VminusY) = colorDifferenceIndex.VminusY;

    sync();
    syncWithConfig();

    DebugParameterList::getInstance().add(this);
  }

  ~ColorRegionParameter()
  {
    DebugParameterList::getInstance().remove(this);
  } 

  inline bool isInUV(const Pixel& pixel)
  {
    return chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }

  inline bool isInYUV(const Pixel& pixel)
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }

  inline bool isInY(const int& value)
  {
    return chLow.Y <= value && value <= chHigh.Y;
  }

  inline bool isInU(const int& value)
  {
    return chLow.U <= value && value <= chHigh.U;
  }

  inline bool isInV(const int& value)
  {
    return chLow.V <= value && value <= chHigh.V;
  }

  inline bool isInVminusU(const int& value)
  {
    return cDiffLow.VminusU <= value && value <= cDiffHigh.VminusU;
  }

  inline bool isInUminusY(const int& value)
  {
    return cDiffLow.UminusY <= value && value <= cDiffHigh.UminusY;
  }

  inline bool isInVminusY(const int& value)
  {
    return cDiffLow.VminusY <= value && value <= cDiffHigh.VminusY;
  }
};

class ParamColorRegion : public ColorRegion, public ParameterList
{
public:
  
  ParamColorRegion(std::string regionName, ColorClasses::Color regionColor)
  : 
    ColorRegion(regionColor),
    ParameterList(std::string( ColorClasses::getColorName(regionColor)) + "ColorRegion_" + regionName)
  {
    if
    (
      regionColor == ColorClasses::orange
      ||
      regionColor == ColorClasses::yellow
      ||
      regionColor == ColorClasses::skyblue
    )
    {
      //colorChannelDistance.Y = 128;
      PARAMETER_REGISTER(colorChannelDistance.Y) = colorChannelDistance.Y;        
      PARAMETER_REGISTER(colorChannelDistance.U) = colorChannelDistance.U;        
      PARAMETER_REGISTER(colorChannelDistance.V) = colorChannelDistance.V;
      //colorChannelIndex.Y = 127;
      PARAMETER_REGISTER(colorChannelIndex.Y) = colorChannelIndex.Y;        
      PARAMETER_REGISTER(colorChannelIndex.U) = colorChannelIndex.U;        
      PARAMETER_REGISTER(colorChannelIndex.V) = colorChannelIndex.V;
      PARAMETER_REGISTER(colorDifferenceDistance.VminusU) = colorDifferenceDistance.VminusU;
      PARAMETER_REGISTER(colorDifferenceIndex.VminusU) = colorDifferenceIndex.VminusU;
    }
    else  if
    (
      regionColor == ColorClasses::pink
      ||
      regionColor == ColorClasses::blue
      ||
      regionColor == ColorClasses::green
      ||
      regionColor == ColorClasses::white
    )
    {
      PARAMETER_REGISTER(colorChannelDistance.Y) = colorChannelDistance.Y;        
      PARAMETER_REGISTER(colorChannelDistance.U) = colorChannelDistance.U;        
      PARAMETER_REGISTER(colorChannelDistance.V) = colorChannelDistance.V;        
      PARAMETER_REGISTER(colorChannelIndex.Y) = colorChannelIndex.Y;        
      PARAMETER_REGISTER(colorChannelIndex.U) = colorChannelIndex.U;        
      PARAMETER_REGISTER(colorChannelIndex.V) = colorChannelIndex.V;        
    }
    else
    {
      PARAMETER_REGISTER(colorChannelDistance.Y) = colorChannelDistance.Y;        
      PARAMETER_REGISTER(colorChannelDistance.U) = colorChannelDistance.U;        
      PARAMETER_REGISTER(colorChannelDistance.V) = colorChannelDistance.V;        
      PARAMETER_REGISTER(colorChannelIndex.Y) = colorChannelIndex.Y;        
      PARAMETER_REGISTER(colorChannelIndex.U) = colorChannelIndex.U;        
      PARAMETER_REGISTER(colorChannelIndex.V) = colorChannelIndex.V;        

      PARAMETER_REGISTER(colorDifferenceDistance.VminusU) = colorDifferenceDistance.VminusU;        
      PARAMETER_REGISTER(colorDifferenceDistance.UminusY) = colorDifferenceDistance.UminusY;        
      PARAMETER_REGISTER(colorDifferenceDistance.VminusY) = colorDifferenceDistance.VminusY;        
      PARAMETER_REGISTER(colorDifferenceIndex.VminusU) = colorDifferenceIndex.VminusU;        
      PARAMETER_REGISTER(colorDifferenceIndex.UminusY) = colorDifferenceIndex.UminusY;        
      PARAMETER_REGISTER(colorDifferenceIndex.VminusY) = colorDifferenceIndex.VminusY;
    }
    sync();
    syncWithConfig();

    DebugParameterList::getInstance().add(this);
  }

  ~ParamColorRegion()
  {
    DebugParameterList::getInstance().remove(this);
  } 
};


class orangeColorRegion : public ColorRegion
{
public:
  
  orangeColorRegion()
  :
  ColorRegion(ColorClasses::orange)
  {}

  inline bool inside(const Pixel& pixel) const
  {
    int diff = pixel.v - pixel.u;
    return chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V &&
            cDiffLow.VminusU <= diff && diff <= cDiffHigh.VminusU;
  }
};

class orangeParamColorRegion : public ParamColorRegion
{
public:
  
  orangeParamColorRegion(std::string regionName)
  : 
    ParamColorRegion(regionName, ColorClasses::orange)
  {}

  inline bool inside(const Pixel& pixel)
  {
    int diff = pixel.v - pixel.u;
    return chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V &&
            cDiffLow.VminusU <= diff && diff <= cDiffHigh.VminusU;
  }
};

class yellowColorRegion : public ColorRegion
{
public:
  
  yellowColorRegion()
  : 
    ColorRegion(ColorClasses::yellow)
  {}

  inline bool inside(const Pixel& pixel) const
  {
    int diff = pixel.v - pixel.u;
    return chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V && 
            chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            cDiffLow.VminusU <= diff && diff <= cDiffHigh.VminusU;
  }
};

class yellowParamColorRegion : public ParamColorRegion
{
public:
  
  yellowParamColorRegion(std::string regionName)
  : 
    ParamColorRegion(regionName, ColorClasses::yellow)
  {}

  inline bool inside(const Pixel& pixel)
  {
    int diff = pixel.v - pixel.u;
    return chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V && 
            cDiffLow.VminusU <= diff && diff <= cDiffHigh.VminusU;
  }
};

class skyblueColorRegion : public ColorRegion
{
public:
  
  skyblueColorRegion()
  : 
    ColorRegion(ColorClasses::skyblue)
  {}

  inline bool inside(const Pixel& pixel) const
  {
    int diff = pixel.v - pixel.u;
    return chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V && 
            cDiffLow.VminusU <= diff && diff <= cDiffHigh.VminusU;
  }
};

class skyblueParamColorRegion : public ParamColorRegion
{
public:
  
  skyblueParamColorRegion(std::string regionName)
  : 
    ParamColorRegion(regionName, ColorClasses::skyblue)
  {}

  inline bool inside(const Pixel& pixel)
  {
    int diff = pixel.v - pixel.u;
    return chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V && 
            cDiffLow.VminusU <= diff && diff <= cDiffHigh.VminusU;
  }
};

class pinkColorRegion : public ColorRegion
{
public:
  
  pinkColorRegion()
  : 
    ColorRegion(ColorClasses::pink)
  {}

  inline bool inside(const Pixel& pixel) const
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
};

class pinkParamColorRegion : public ParamColorRegion
{
public:
  
  pinkParamColorRegion(std::string regionName)
  : 
    ParamColorRegion(regionName, ColorClasses::pink)
  {}

  inline bool inside(const Pixel& pixel)
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
};

class blueColorRegion : public ColorRegion
{
public:
  
  blueColorRegion()
  : 
    ColorRegion(ColorClasses::blue)
    {}

  inline bool inside(const Pixel& pixel) const
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
};

class blueParamColorRegion : public ParamColorRegion
{
public:
  
  blueParamColorRegion(std::string regionName)
  : 
    ParamColorRegion(regionName, ColorClasses::blue)
    {}

  inline bool inside(const Pixel& pixel)
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
};

class greenColorRegion : public ColorRegion
{
public:
  
  greenColorRegion()
  : 
    ColorRegion(ColorClasses::green)
    {}

  inline bool inside(const Pixel& pixel) const
  {
    return pixel.y <= chHigh.Y && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
};

class greenParamColorRegion : public ParamColorRegion
{
public:
  
  greenParamColorRegion(std::string regionName)
  : 
    ParamColorRegion(regionName, ColorClasses::green)
    {}

  inline bool inside(const Pixel& pixel)
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
};

class whiteColorRegion : public ColorRegion
{
public:
  
  whiteColorRegion()
  : 
    ColorRegion(ColorClasses::white)
    {}

  inline bool inside(const Pixel& pixel) const
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
};

class whiteParamColorRegion : public ParamColorRegion
{
public:
  
  whiteParamColorRegion(std::string regionName)
  : 
    ParamColorRegion(regionName, ColorClasses::white)
    {}

  inline bool inside(const Pixel& pixel)
  {
    return chLow.Y <= pixel.y && pixel.y <= chHigh.Y && 
            chLow.U <= pixel.u && pixel.u <= chHigh.U && 
            chLow.V <= pixel.v && pixel.v <= chHigh.V;
  }
};


#endif // COLORREGION_H
