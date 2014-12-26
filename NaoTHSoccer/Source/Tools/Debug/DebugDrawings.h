// 
// File:   DebugDrawings.h
// Author: thomas
//
// Created on 19. march 2008, 21:51
//

#ifndef _DebugDrawings_H
#define _DebugDrawings_H

#include <cstring>
#include <sstream>
#include <iostream>

#include "Tools/Math/Vector_n.h"

#include <DebugCommunication/DebugCommandExecutor.h>
#include "Tools/DataStructures/Serializer.h"
#include "Color.h"

class DrawingCanvas2D
{
public:

  void reset() {
    buffer.str("");
    buffer.clear();
  }

  std::stringstream& out() {
    return buffer;
  }
  const std::stringstream& out() const {
    return buffer;
  }

public:
  void pen(const char* color, double width) {
    out() << "Pen:" << color << ":" << width << std::endl;
  }
  void pen(const std::string& color, double width) {
    out() << "Pen:" << color << ":" << width << std::endl;
  }
  void pen(const Color& color, double width) {
    out() << "Pen:" << color << ":" << width << std::endl;
  }
  void drawCircle(double x, double y, double radius) {
    out() << "Circle:" << x << ":" << y << ":" << radius << ":" <<  std::endl;
  }
  void drawLine(double x0, double y0, double x1, double y1) {
    out() << "Line:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  std::endl;
  }
  void drawText(double x, double y, const char* text) {
    out() << "Text:" << x << ":" << y << ":" << text << ":" <<  std::endl;
  }

  void fillBox(double x0, double y0, double x1, double y1) {
    out() << "FillBox:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" << std::endl;
  }
  void fillOval(double x, double y, double radiusX, double radiusY) {
    out() << "FillOval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" <<  std::endl;
  }

  

private:
  std::stringstream buffer;
};

class DebugDrawings : public DrawingCanvas2D {};

namespace naoth
{
template<>
class Serializer<DrawingCanvas2D>
{
  public:
  static void serialize(const DrawingCanvas2D& object, std::ostream& stream);
  static void deserialize(std::istream& stream, DrawingCanvas2D& object);
};

template<> class Serializer<DebugDrawings> : public Serializer<DrawingCanvas2D> {};
}


#ifdef DEBUG
#define CANVAS(id) getDebugDrawings().out() << "Canvas:" << id << std::endl
#define IMAGE_DRAWING_CONTEXT getDebugDrawings().out() << "DrawingOnImage" << std::endl
#define FIELD_DRAWING_CONTEXT getDebugDrawings().out() << "DrawingOnField" << std::endl

#define PEN(color, width) getDebugDrawings().out() << "Pen:" << color << ":" << width << std::endl
#define ROTATION(angle) getDebugDrawings().out() << "Rotation:" << angle << std::endl
#define TRANSLATION(x,y) getDebugDrawings().out() << "Translation:" << x << ":" << y << std::endl

#define CIRCLE(x,y,radius) getDebugDrawings().out() << "Circle:" << x << ":" << y << ":" << radius << ":" <<  std::endl
#define OVAL(x,y,radiusX,radiusY) getDebugDrawings().out() << "Oval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" <<  std::endl
#define OVAL_ROTATED(x,y,radiusX,radiusY,rotation) getDebugDrawings().out() << "Oval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" << rotation << ":" <<  std::endl
#define ARROW(x0,y0,x1,y1) getDebugDrawings().out() << "Arrow:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  std::endl
#define LINE(x0,y0,x1,y1) getDebugDrawings().out() << "Line:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  std::endl
#define BOX(x0,y0,x1,y1) getDebugDrawings().out() << "Box:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" <<  std::endl
#define FILLBOX(x0,y0,x1,y1) getDebugDrawings().out() << "FillBox:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << ":" << std::endl
#define FILLOVAL(x,y,radiusX,radiusY) getDebugDrawings().out() << "FillOval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" <<  std::endl
#define TEXT_DRAWING(x,y,text) getDebugDrawings().out() << "Text:" << x << ":" << y << ":" << text << ":" <<  std::endl
#define SIMPLE_PARTICLE(x,y,r) getDebugDrawings().out() << "Particle:" << x << ":" << y << ":" << r << ":" <<  std::endl

#define PARTICLE(x,y,r,l) \
  LINE(x,y,x + l*cos(r),y + l*sin(r)); \
  CIRCLE(x,y,0.1*l)

#define BINARY_PLOT(name,value) getDebugDrawings().out() << "BinaryPlotData:" << name << ":" << value << std::endl
#define ROBOT(x,y,rotation) getDebugDrawings().out() << "Robot:" << x << ":" << y << ":" << rotation << ":" <<  std::endl


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

