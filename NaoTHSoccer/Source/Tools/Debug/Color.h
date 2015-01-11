
#ifndef _Color_H_
#define _Color_H_

#include <cstring>
#include <sstream>
#include <iostream>

#include "Tools/Math/Vector_n.h"

/**
* class Color encodes a color by 4 double values.
* The values represent red, green, blue and the alpha chanels.
* every value has to be between 0.0 and 1.0
*/
class Color: public Vector_n<double,4>
{
public:
  enum Chanel
  {
    red,
    green, 
    blue,
    alpha,
    numberOfChanel
  };

  // default color is black
  Color();

  Color(const Vector_n<double,4>& colorVector);

  Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

  Color(double r, double g, double b, double a = 1.0);

  Color(const char* color);

  /**
  * Generate a color using an index number.
  *
  * The generation is done by more or less random parameters which showed that
  * they generate differing colors for the first indexes.
  */
  Color(unsigned int colorIndex);

  unsigned char getRed() const;
  unsigned char getGreen() const;
  unsigned char getBlue() const;
  unsigned char getAlpha() const;

  // html style hex string
  std::string toString() const;

  static std::string charToHexString(unsigned char c);
  static int hexCharToInt(char c);
  static char intToHexChar(int n);
};//end class Color

/**
 * Streaming operator that writes a Color to a stream.
 * @param stream The stream to write on.
 * @param image The Color object.
 * @return The stream.
 */ 
std::ostream& operator<<(std::ostream& stream, const Color& color);

#endif  /* _Color_H_ */

