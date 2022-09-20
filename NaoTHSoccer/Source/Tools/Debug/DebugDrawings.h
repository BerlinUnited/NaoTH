// 
// File:   DebugDrawings.h
// Author: Thomas Krause
// Author: Heinrich Mellmann <mellmann@informatik.hu-berlin.de>
//
// Created on 19. march 2008, 21:51
//

#ifndef DEBUG_DRAWINGS_H
#define DEBUG_DRAWINGS_H

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
    out() << "Pen:" << color << ":" << width << '\n';
  }
  void pen(const std::string& color, double width) {
    out() << "Pen:" << color << ":" << width << '\n';
  }
  void pen(const Color& color, double width) {
    out() << "Pen:" << color << ":" << width << '\n';
  }
  void drawCircle(double x, double y, double radius) {
    out() << "Circle:" << x << ":" << y << ":" << radius << '\n';
  }
  void drawLine(double x0, double y0, double x1, double y1) {
    out() << "Line:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << '\n';
  }
  void drawArrow(double x0, double y0, double x1, double y1) {
    out() << "Arrow:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << '\n';
  }
  void drawText(double x, double y, const char* text) {
    out() << "Text:" << x << ":" << y << ":" << text << '\n';
  }
  void drawText(double x, double y, const std::string& text) {
    out() << "Text:" << x << ":" << y << ":" << text << ":" <<  std::endl;
  }

  void drawBox(double x0, double y0, double x1, double y1) {
    out() << "Box:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << '\n';
  }
  void fillBox(double x0, double y0, double x1, double y1) {
    out() << "FillBox:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << '\n';
  }
  void fillOval(double x, double y, double radiusX, double radiusY) {
    out() << "FillOval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << '\n';
  }

  void drawOvalRotated(double x,double y,double radiusX,double radiusY,double rotation) {
    out() << "Oval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" << rotation << '\n';
  }

  void drawRobot(double x, double y, double rotation){
    out() << "Robot:" << x << ":" << y << ":" << rotation << '\n';
  }

  void rotate(double angle) {
    out() << "Rotation:" << angle << '\n';
  }
  void translate(double x, double y) {
    out() << "Translation:" << x << ":" << y << '\n';
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
#define CANVAS(id) getDebugDrawings().out() << "Canvas:" << id << '\n'
#define IMAGE_DRAWING_CONTEXT getDebugDrawings().out() << "DrawingOnImage" << '\n'
#define FIELD_DRAWING_CONTEXT getDebugDrawings().out() << "DrawingOnField" << '\n'

#define PEN(color, width) getDebugDrawings().out() << "Pen:" << color << ":" << width << '\n'
#define PEN_DASHED(color, width, dash_length) getDebugDrawings().out() << "Pen:" << color << ":" << width << ":" << dash_length << '\n'
#define ROTATION(angle) getDebugDrawings().out() << "Rotation:" << angle << '\n'
#define TRANSLATION(x,y) getDebugDrawings().out() << "Translation:" << x << ":" << y << '\n'

#define CIRCLE(x,y,radius) getDebugDrawings().out() << "Circle:" << x << ":" << y << ":" << radius << '\n'
#define OVAL(x,y,radiusX,radiusY) getDebugDrawings().out() << "Oval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << '\n'
#define OVAL_ROTATED(x,y,radiusX,radiusY,rotation) getDebugDrawings().out() << "Oval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << ":" << rotation << '\n'
#define ARROW(x0,y0,x1,y1) getDebugDrawings().out() << "Arrow:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << '\n'
#define LINE(x0,y0,x1,y1) getDebugDrawings().out() << "Line:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << '\n'
#define BOX(x0,y0,x1,y1) getDebugDrawings().out() << "Box:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << '\n'
#define FILLBOX(x0,y0,x1,y1) getDebugDrawings().out() << "FillBox:" << x0 << ":" << y0 << ":" << x1 << ":" << y1 << '\n'
#define FILLOVAL(x,y,radiusX,radiusY) getDebugDrawings().out() << "FillOval:" << x << ":" << y << ":" << radiusX << ":" << radiusY << '\n'
#define TEXT_DRAWING(x,y,text) getDebugDrawings().out() << "Text:" << x << ":" << y << ":" << text << '\n'
#define TEXT_DRAWING2(x,y,s,text) getDebugDrawings().out() << "Text:" << x << ":" << y << ":" << s << ":" << text << '\n'
#define SIMPLE_PARTICLE(x,y,r) getDebugDrawings().out() << "Particle:" << x << ":" << y << ":" << r << '\n'
// voronoi stuff
#define FILLPOLYGON(x,y) getDebugDrawings().out() << "FillPolygon:" << x << ":" << y << '\n'
#define POLYGON(x,y) getDebugDrawings().out() << "Polygon:" << x << ":" << y << '\n'
// voronoi stuff end
#define PARTICLE(x,y,r,l) \
  LINE(x,y,x + l*cos(r),y + l*sin(r)); \
  CIRCLE(x,y,0.1*l)

#define BINARY_PLOT(name,value) getDebugDrawings().out() << "BinaryPlotData:" << name << ":" << value << '\n'
#define ROBOT(x,y,rotation) getDebugDrawings().out() << "Robot:" << x << ":" << y << ":" << rotation << '\n'
#define ROBOT_WITH_HEAD(x,y,rotation, headRotation) getDebugDrawings().out() << "Robot:" << x << ":" << y << ":" << rotation << ":" <<  headRotation << '\n'


#else
/* ((void)0) - that's a do-nothing statement */
#define IMAGE_DRAWING_CONTEXT ((void)0)
#define FIELD_DRAWING_CONTEXT ((void)0)
#define PEN(color, width) ((void)0)
#define PEN_DASHED(color, width, dash_length) ((void)0)
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
#define TEXT_DRAWING2(x,y,s,text) ((void)0)

#define PARTICLE(x,y,r,l) ((void)0)

#define ROBOT(x,y,rotation) ((void)0)
#endif //DEBUG

#endif  /* DEBUG_DRAWINGS_H */

