// 
// File:   DebugDrawings.h
// Author: thomas
//
// Created on 19. März 2008, 21:51
//

#ifndef _DebugDrawings_H
#define _DebugDrawings_H

#include <cstring>
#include <sstream>
#include <iostream>

#include "Tools/DataStructures/Singleton.h"
#include "Tools/Math/Vector_n.h"

#include <DebugCommunication/DebugCommandExecutor.h>

class DebugDrawings : public naoth::Singleton<DebugDrawings>, public DebugCommandExecutor
{
protected:
  friend class naoth::Singleton<DebugDrawings>;
  DebugDrawings();
  ~DebugDrawings();

public:

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);
  
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
std::ostream& operator<<(std::ostream& stream, const DebugDrawings::Color& color);


#ifdef DEBUG
#define IMAGE_DRAWING_CONTEXT DebugDrawings::getInstance().out() << "DrawingOnImage" << std::endl
#define FIELD_DRAWING_CONTEXT DebugDrawings::getInstance().out() << "DrawingOnField" << std::endl
#define PEN(color, width) DebugDrawings::getInstance().out() << "Pen:" << color << ":" << width << std::endl
#define ROTATION(angle) DebugDrawings::getInstance().out() << "Rotation:" << angle << std::endl
#define TRANSLATION(x,y) DebugDrawings::getInstance().out() << "Translation:" << x << ":" << y << std::endl

#define CIRCLE(x,y,radius) DebugDrawings::getInstance().out() << "Circle:" << x << ":" << y << ":" << radius << ":" <<  std::endl
#define OVAL(x,y,radiusX,radiusY) DebugDrawings::getInstance().out() << "Oval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" <<  std::endl
#define OVAL_ROTATED(x,y,radiusX,radiusY,rotation) DebugDrawings::getInstance().out() << "Oval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" << rotation << ":" <<  std::endl
#define ARROW(x0,y0,x1,y1) DebugDrawings::getInstance().out() << "Arrow:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  std::endl
#define LINE(x0,y0,x1,y1) DebugDrawings::getInstance().out() << "Line:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  std::endl
#define BOX(x0,y0,x1,y1) DebugDrawings::getInstance().out() << "Box:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  std::endl
#define FILLBOX(x0,y0,x1,y1) DebugDrawings::getInstance().out() << "FillBox:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" << std::endl
#define FILLOVAL(x,y,radiusX,radiusY) DebugDrawings::getInstance().out() << "FillOval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" <<  std::endl
#define TEXT_DRAWING(x,y,text) DebugDrawings::getInstance().out() << "Text:" << x << ":" << y << ":" << text << ":" <<  std::endl
#define SIMPLE_PARTICLE(x,y,r) DebugDrawings::getInstance().out() << "Particle:" << x << ":" << y << ":" << r << ":" <<  std::endl

#define PARTICLE(x,y,r,l) \
  LINE(x,y,x + l*cos(r),y + l*sin(r)); \
  CIRCLE(x,y,0.1*l)

#define BINARY_PLOT(name,value) DebugDrawings::getInstance().out() << "BinaryPlotData:" << name << ":" << value << std::endl
#define ROBOT(x,y,rotation) DebugDrawings::getInstance().out() << "Robot:" << x << ":" << y << ":" << rotation << ":" <<  std::endl


#else
/* ((void)0) - that's a do-nothing statement */
#define IMAGE_DRAWING_CONTEXT ((void)0)
#define FIELD_DRAWING_CONTEXT ((void)0)
#define PEN(color, width) ((void)0)
#define ROTATION(angle) ((void)0)
#define TRANSLATION(x,y) ((void)0)

#define CIRCLE(x,y,radius) ((void)0)
#define OVAL(x,y,radiusX,radiusY) ((void)0)
#define OVAL_ROTATED(x,y,radiusX,radiusY,rotation) ((void)0)
#define ARROW(x0,y0,x1,y1) ((void)0)
#define LINE(x0,y0,x1,y1) ((void)0)
#define BOX(x0,y0,x1,y1) ((void)0)
#define FILLBOX(x0,y0,x1,y1) ((void)0)
#define FILLOVAL(x,y,radiusX,radiusY) ((void)0)
#define TEXT_DRAWING(x,y,text) ((void)0)

#define PARTICLE(x,y,r,l) ((void)0)

#define ROBOT(x,y,rotation) ((void)0)
#endif //DEBUG

#endif  /* _DebugDrawings_H */

