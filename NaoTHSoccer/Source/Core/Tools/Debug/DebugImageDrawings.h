// 
// File:   DebugImageDrawings.h
// Author: Heinrich Mellmann
//
// Created on 19. march 2008, 21:51
//

#ifndef _DebugImageDrawings_H
#define _DebugImageDrawings_H

#include <Tools/DataStructures/Singleton.h>
#include <Tools/ImageProcessing/ImageDrawings.h>
#include <Representations/Infrastructure/CameraInfo.h>

#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/Math/Vector2.h>
#include "Tools/ColorClasses.h"

#include <vector>


class DebugDrawingCanvas: public naoth::DrawingCanvas
{
public:
  DebugDrawingCanvas()
  : 
    pixels(width()*height()),
    coordinates(width()*height()),
    numberOfPoints(0)
  {

    for(unsigned int i = 0; i < pixels.size(); i++)
    {
        pixels[i].a = 0;
        pixels[i].b = 0;
        pixels[i].c = 0;
    }
  };

  ~DebugDrawingCanvas(){}

public:
  virtual void drawPoint(
    const unsigned int& x, 
    const unsigned int& y,
    const unsigned char& a,
    const unsigned char& b,
    const unsigned char& c)
  {
    if((unsigned int)numberOfPoints < width()*height())
    {
      coordinates[numberOfPoints].x = x;
      coordinates[numberOfPoints].y = y;
      pixels[numberOfPoints].a = a;
      pixels[numberOfPoints].b = b;
      pixels[numberOfPoints].c = c;
      numberOfPoints++;
    }//end if
  }

  unsigned int width() const {
    return naoth::IMAGE_WIDTH;
  }

  unsigned int height() const {
    return naoth::IMAGE_HEIGHT;
  }

  void reset() {
    numberOfPoints = 0;
  }

  void drawToImage(DrawingCanvas& image)
  {
    for(int i = 0; i < numberOfPoints; i++)
    {
      image.drawPoint(
        coordinates[i].x,
        coordinates[i].y,
        pixels[i].a,
        pixels[i].b,
        pixels[i].c);
    }//end for
  }

  // drawing methods for the color classes
  inline void drawCircleToImage(
    ColorClasses::Color color, 
    const unsigned int startX,
    const unsigned int startY,
    const unsigned int radius)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawCircleToImage(*this, startX, startY, radius, yy, cb, cr);
  }

  inline void drawRectToImage(
    ColorClasses::Color color, 
    const unsigned int startX,
    const unsigned int startY,
    const unsigned int endX,
    const unsigned int endY)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawRectToImage(*this, startX, startY, endX, endY, yy, cb, cr);
  }

  inline void drawLineToImage(
    ColorClasses::Color color, 
    const unsigned int startX,
    const unsigned int startY,
    const unsigned int endX,
    const unsigned int endY)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawLineToImage(*this, startX, startY, endX, endY, yy, cb, cr);
  }

  inline void drawPointToImage(
    ColorClasses::Color color, 
    const unsigned int x,
    const unsigned int y)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawPointToImage(*this, x, y, yy, cb, cr);
  }

  inline void drawPointToImage(
    const unsigned char yy,
    const unsigned char cb,
    const unsigned char cr, 
    const unsigned int x,
    const unsigned int y)
  {
    naoth::ImageDrawings::drawPointToImage(*this, x, y, yy, cb, cr);
  }

private:
  std::vector<Pixel> pixels;
  std::vector<Vector2<unsigned int> > coordinates;
  int numberOfPoints;
};


class DebugBottomImageDrawings : public naoth::Singleton<DebugBottomImageDrawings>
{
protected:
  friend class naoth::Singleton<DebugBottomImageDrawings>;
  DebugBottomImageDrawings()
    : currentCanvas(&canvasBottom)
  {
  }

public:
  void setCanvas(naoth::CameraInfo::CameraID id) {
    if(id == naoth::CameraInfo::Bottom) {
      currentCanvas = &canvasBottom;
    } else if(id == naoth::CameraInfo::Top) {
      currentCanvas = &canvasTop;
    } else {
      assert(false);
    }
  }

  DebugDrawingCanvas& canvas() {
    return *currentCanvas;
  }

  DebugDrawingCanvas& canvas(naoth::CameraInfo::CameraID id) {
    if(id == naoth::CameraInfo::Bottom) {
      return canvasBottom;
    } else {
      return canvasTop;
    }
  }

  void reset() {
    canvasTop.reset();
    canvasBottom.reset();
  }

private:
  DebugDrawingCanvas canvasTop;
  DebugDrawingCanvas canvasBottom;
  DebugDrawingCanvas* currentCanvas;
};

#ifdef DEBUG
#define CANVAS_PX(id) DebugBottomImageDrawings::getInstance().setCanvas(id)
#define CANVAS_PX_TOP DebugBottomImageDrawings::getInstance().setCanvas(naoth::CameraInfo::Top)
#define CANVAS_PX_BOTTOM DebugBottomImageDrawings::getInstance().setCanvas(naoth::CameraInfo::Bottom)
#define CIRCLE_PX(...) DebugBottomImageDrawings::getInstance().canvas().drawCircleToImage(__VA_ARGS__)
#define RECT_PX(...) DebugBottomImageDrawings::getInstance().canvas().drawRectToImage(__VA_ARGS__)
#define LINE_PX(...) DebugBottomImageDrawings::getInstance().canvas().drawLineToImage(__VA_ARGS__)
#define POINT_PX(...) DebugBottomImageDrawings::getInstance().canvas().drawPointToImage(__VA_ARGS__)
#else
/* ((void)0) - that's a do-nothing statement */
#define CIRCLE_PX(color,x,y,radius) ((void)0)
#define RECT_PX(color,x0,y0,x1,y1) ((void)0)
#define LINE_PX(color,x0,y0,x1,y1) ((void)0)
#define POINT_PX(color,x,y) ((void)0)
#endif //DEBUG

class DebugTopImageDrawings : public naoth::Singleton<DebugTopImageDrawings>, public DebugDrawingCanvas
{
protected:
  friend class naoth::Singleton<DebugTopImageDrawings>;
};

#ifdef DEBUG
#define TOP_CIRCLE_PX(...) DebugTopImageDrawings::getInstance().drawCircleToImage(__VA_ARGS__)
#define TOP_RECT_PX(...) DebugTopImageDrawings::getInstance().drawRectToImage(__VA_ARGS__)
#define TOP_LINE_PX(...) DebugTopImageDrawings::getInstance().drawLineToImage(__VA_ARGS__)
#define TOP_POINT_PX(...) DebugTopImageDrawings::getInstance().drawPointToImage(__VA_ARGS__)
#else
/* ((void)0) - that's a do-nothing statement */
#define TOP_CIRCLE_PX(color,x,y,radius) ((void)0)
#define TOP_RECT_PX(color,x0,y0,x1,y1) ((void)0)
#define TOP_LINE_PX(color,x0,y0,x1,y1) ((void)0)
#define TOP_POINT_PX(color,x,y) ((void)0)
#endif //DEBUG

#endif  /* _DebugImageDrawings_H */
