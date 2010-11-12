/**
* @file ColorClasses.h
* Declaration of class ColorClasses
*
* @author <A href="mailto:mellmann@informatik.hu-berlin.de">Heinrch Mellmann</A>
*/

#ifndef __ColorClasses_h__
#define __ColorClasses_h__

class ColorClasses
{
public:
  enum Color
  {
    none, /*<! all other objects */
    orange, /*<! ball */
    yellow, /*<! yellow goal and flag */
    skyblue, /*<! skyblue goal and flag */
    white, /*<! boundaries and lines, player skin */
    red, /*<! red player */
    blue, /*<! blue player */
    green, /*<! playground and green flag */
    black, /*<! player joints, ball-challenge */
    pink, /*<! pink flag */
    gray, /*<! player */
    yellowOrange, /*<! ball or yellow goal */
    numOfColors /*<! number of colors */
  };

  /**
   * Returns the name of a color class
   * @param color The color class
   * @return The name
   */
  static const char* getColorName(Color color)
  {
    switch(color) 
    {
      case none: return "none";
      case orange: return "orange"; 
      case yellow: return "yellow"; 
      case skyblue: return "skyblue"; 
      case pink: return "pink"; 
      case red: return "red";
      case blue: return "blue";
      case green: return "green"; 
      case gray: return "gray";
      case white: return "white";
      case black: return "black";
      case yellowOrange: return "yellowOrange"; 
      default: return "unknown color!"; 
    };
  }//end getColorName


  /**
   * Returns the Hex-string representing the color class
   * @param color The color class
   * @return Hex-string
   */
  static const char* colorClassToHex(Color color)
  {
    switch(color) 
    {
      case orange: return "FFC800";
      case yellow: return "FFFF00"; 
      case skyblue: return "8080FF";
      case pink: return "FF00FF"; 
      case red: return "FF0000";
      case blue: return "000080";
      case green: return "00FF00"; 
      case gray: return "808080";
      case white: return "FFFFFF";
      case black: return "000000";
      case yellowOrange: return "FFDA4A";
      default: return "99739A";
    };
  }//end getColorName


 /**
  * Returns the RGB-Value for a color class
  * @param color The color class
  * @param r Reference to the red value
  * @param g Reference to the green value
  * @param b Reference to the blue value
  */
  static void colorClassToRGB(Color color, unsigned char& r, unsigned char& g, unsigned char& b)
  {
    switch(color) 
    {
      case orange:  r = 255; g = 200; b = 0;  break;
      case yellow:  r = 255; g = 255; b = 0;   break;
      case skyblue: r = 128;   g = 128; b = 255; break;
      case pink:    r = 255; g = 0;   b = 255; break;
      case red:     r = 255; g = 0;   b = 0;   break;
      case blue:    r = 0;   g = 0;   b = 128; break;
      case gray:    r = 128; g = 128; b = 128; break;
      case green:   r = 0;   g = 255; b = 0;   break;
      case white:   r = 255; g = 255; b = 255; break;
      case black:   r = 0; g = 0; b = 0; break;
      case yellowOrange:   r = 255; g = 218; b = 74; break;
      default: r = 153; g = 115; b = 154; break;
    };
  }//end colorClassToRGB
};

#endif //__ColorClasses_h_
