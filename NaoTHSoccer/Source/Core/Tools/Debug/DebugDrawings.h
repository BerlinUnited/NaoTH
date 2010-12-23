// 
// File:   DebugDrawings.h
// Author: thomas
//
// Created on 19. März 2008, 21:51
//

#ifndef _DebugDrawings_H
#define	_DebugDrawings_H

#include <cstring>
#include <sstream>
#include <iostream>

#include "Tools/DataStructures/Singleton.h"
#include "Tools/Math/Vector_n.h"
#include "Tools/Math/Common.h"
#include <DebugCommandExecutor.h>

using namespace std;
using namespace naoth;

class DebugDrawings : public Singleton<DebugDrawings>, public DebugCommandExecutor
{
protected:
  friend class Singleton<DebugDrawings>;
  DebugDrawings();
  ~DebugDrawings();

public:

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream);
  
  void update();
  std::stringstream& out();

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
    Color()
    {
      (*this)[red]   = 0.0;
      (*this)[green] = 0.0;
      (*this)[blue]  = 0.0;
      (*this)[alpha] = 1.0;
    }

    Color(const Vector_n<double,4>& colorVector)
      : Vector_n<double,4>(colorVector)
    {
    }

    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
    {
      (*this)[red]   = static_cast<double>(r)/255.0;
      (*this)[green] = static_cast<double>(g)/255.0;
      (*this)[blue]  = static_cast<double>(b)/255.0;
      (*this)[alpha] = static_cast<double>(a)/255.0;
    }

    Color(double r, double g, double b, double a = 1.0)
    {
      (*this)[red]   = r;
      (*this)[green] = g;
      (*this)[blue]  = b;
      (*this)[alpha] = a;
    }

    Color(const char* color)
    {
      (*this)[red]   = 16*hexCharToInt(color[0]) + hexCharToInt(color[1]);
      (*this)[green] = 16*hexCharToInt(color[2]) + hexCharToInt(color[3]);
      (*this)[blue]  = 16*hexCharToInt(color[4]) + hexCharToInt(color[5]);
      if(strlen(color) == 8)
        (*this)[alpha] = 16*hexCharToInt(color[6]) + hexCharToInt(color[7]);
    }

    /**
     * Generate a color using an index number.
     *
     * The generation is done by more or less random parameters which showed that
     * they generate differing colors for the first indexes.
     */
    Color(unsigned int colorIndex)
    {
      unsigned int r = (((colorIndex)*224) % 255);
      unsigned int g = (((colorIndex + 197)*1034345) % 255);
      unsigned int b = (((colorIndex + 23)*74353) % 255);

      //  too dark or too bright?
      if(((r+b+g) / 3) < 100 )
      {
        r = 255 - r;
        g = 255 - g;
        b = 255 - b;
      }
      else if(((r+b+g) / 3) > 192 )
      {
        r = 1*(r / 2);
        g = 1*(g / 2);
        b = 1*(b / 2);
      }

      if(r == 255 && g == 255 && b == 255)
      {
        r = 255;
        g = 255;
        b = 0;
      }

      (*this)[red] = ((double) r) / 255.0;
      (*this)[green] = ((double) g) / 255.0;
      (*this)[blue] = ((double) b) / 255.0;
      (*this)[alpha] = 1.0;
    }

    unsigned char getRed() const{ return (unsigned char)Math::clamp(((*this)[red]*255.0+0.5),0.0,255.0); }
    unsigned char getGreen() const{ return (unsigned char)Math::clamp(((*this)[green]*255.0+0.5),0.0,255.0); }
    unsigned char getBlue() const{ return (unsigned char)Math::clamp(((*this)[blue]*255.0+0.5),0.0,255.0); }
    unsigned char getAlpha() const{ return (unsigned char)Math::clamp(((*this)[alpha]*255.0+0.5),0.0,255.0); }

    std::string toString() const
    {    
      std::string result;
      result.append(charToHexString(getRed()));
      result.append(charToHexString(getGreen()));
      result.append(charToHexString(getBlue()));
      result.append(charToHexString(getAlpha()));
      return result;
    }//end toString

    static std::string charToHexString(unsigned char c)
    {
      int n = c;
      int second = n%16;
      int first = n/16;

      std::stringstream ss;
      ss << intToHexChar(first) << intToHexChar(second);
      return ss.str();
    }//end charToHexString

    static int hexCharToInt(char c)
    {
      switch(c)
      {
      case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;
      case 'A': return 10;
      case 'B': return 11;
      case 'C': return 12;
      case 'D': return 13;
      case 'E': return 14;
      case 'F': return 15;
      default: return -1;
      }//end switch
    }//end intToHexChar

    static char intToHexChar(int n)
    {
      switch(n)
      {
      case 0: return '0';
      case 1: return '1';
      case 2: return '2';
      case 3: return '3';
      case 4: return '4';
      case 5: return '5';
      case 6: return '6';
      case 7: return '7';
      case 8: return '8';
      case 9: return '9';
      case 10: return 'A';
      case 11: return 'B';
      case 12: return 'C';
      case 13: return 'D';
      case 14: return 'E';
      case 15: return 'F';
      default: return 'X';
      }//end switch

      return 'X';
    }//end intToHexChar
  };

private:

  // reference to the currentelly used buffer
  std::stringstream* debugDrawingsOut;

  // do double-buffering :)
  std::stringstream bufferOne;
  std::stringstream bufferTwo;
};

/**
 * Streaming operator that writes a Color to a stream.
 * @param stream The stream to write on.
 * @param image The Color object.
 * @return The stream.
 */ 
ostream& operator<<(ostream& stream, const DebugDrawings::Color& color);


#ifdef DEBUG
#define IMAGE_DRAWING_CONTEXT DebugDrawings::getInstance().out() << "DrawingOnImage" << endl
#define FIELD_DRAWING_CONTEXT DebugDrawings::getInstance().out() << "DrawingOnField" << endl
#define PEN(color, width) DebugDrawings::getInstance().out() << "Pen:" << color << ":" << width << endl

#define CIRCLE(x,y,radius) DebugDrawings::getInstance().out() << "Circle:" << x << ":" << y << ":" << radius << ":" <<  endl
#define OVAL(x,y,radiusX,radiusY) DebugDrawings::getInstance().out() << "Oval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" <<  endl
#define ARROW(x0,y0,x1,y1) DebugDrawings::getInstance().out() << "Arrow:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  endl
#define LINE(x0,y0,x1,y1) DebugDrawings::getInstance().out() << "Line:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  endl
#define BOX(x0,y0,x1,y1) DebugDrawings::getInstance().out() << "Box:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  endl
#define FILLBOX(x0,y0,x1,y1) DebugDrawings::getInstance().out() << "FillBox:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" << endl
#define FILLOVAL(x,y,radiusX,radiusY) DebugDrawings::getInstance().out() << "FillOval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" <<  endl
#define TEXT_DRAWING(x,y,text) DebugDrawings::getInstance().out() << "Text:" << x << ":" << y << ":" << text << ":" <<  endl

#define ROBOT(x,y,rotation) DebugDrawings::getInstance().out() << "Robot:" << x << ":" << y << ":" << rotation << ":" <<  endl


#else
/* ((void)0) - that's a do-nothing statement */
#define IMAGE_DRAWING_CONTEXT ((void)0)
#define FIELD_DRAWING_CONTEXT ((void)0)
#define PEN(color, width) ((void)0)

#define CIRCLE(x,y,radius) ((void)0)
#define OVAL(x,y,radiusX,radiusY) ((void)0)
#define ARROW(x0,y0,x1,y1) ((void)0)
#define LINE(x0,y0,x1,y1) ((void)0)
#define BOX(x0,y0,x1,y1) ((void)0)
#define FILLBOX(x0,y0,x1,y1) ((void)0)
#define FILLOVAL(x,y,radiusX,radiusY) ((void)0)
#define TEXT_DRAWING(x,y,text) ((void)0)

#define ROBOT(x,y,rotation) ((void)0)
#endif //DEBUG

#endif	/* _DebugDrawings_H */

