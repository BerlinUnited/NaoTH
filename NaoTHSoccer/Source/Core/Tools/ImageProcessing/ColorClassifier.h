

#ifndef __ColorClassifier_h__
#define __ColorClassifier_h__

#include <Tools/ColorClasses.h>
#include <Tools/ImageProcessing/ImagePrimitives.h>


class ColorClassifier
{
public:

  /** 
  * Calculates the color class of a pixel.
  * @param r the r value of the pixel
  * @param g the g value of the pixel
  * @param b the b value of the pixel
  * @return the color class
  */
  inline naoth::ColorClasses::Color getColorClass(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
  {
    return get(a, b, c);
  }

  /**
  * Calculates the color class of a pixel.
  * @param pixel The pixel color
  * @return the color class
  */
  inline naoth::ColorClasses::Color getColorClass(const Pixel& p) const
  {
    return get(p.a, p.b, p.c);
  }

protected:

  virtual naoth::ColorClasses::Color get(const unsigned char& a, const unsigned char& b, const unsigned char& c) const = 0;
};

#endif //__ColorClassifier_h__