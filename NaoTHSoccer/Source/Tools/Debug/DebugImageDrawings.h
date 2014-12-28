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
  DebugDrawingCanvas(int w, int h)
    : numberOfPoints(0)
  {
    init(w,h);
  }

  DebugDrawingCanvas()
    : numberOfPoints(0)
  {
  }

  ~DebugDrawingCanvas(){}

public:

  void init(int w, int h) 
  {
    size = Vector2i(w,h);
    coordinates.resize(w*h);

    // reserve the vector for pixel values and initialize is to (0,0,0)
    Pixel zeroPixel;
    zeroPixel.a = zeroPixel.b = zeroPixel.c = 0;
    pixels.resize(w*h, zeroPixel);
  }

  virtual void drawPoint(
    const int x, 
    const int y,
    const unsigned char a,
    const unsigned char b,
    const unsigned char c)
  {
    if(numberOfPoints < coordinates.size())
    {
      coordinates[numberOfPoints].x = x;
      coordinates[numberOfPoints].y = y;
      pixels[numberOfPoints].a = a;
      pixels[numberOfPoints].b = b;
      pixels[numberOfPoints].c = c;
      numberOfPoints++;
    }
  }

  unsigned int width() const { return size.x; }
  unsigned int height() const { return size.y; }
  void reset() { numberOfPoints = 0; }

  void drawToImage(DrawingCanvas& image)
  {
    for(size_t i = 0; i < numberOfPoints; i++)
    {
      image.drawPoint(
        coordinates[i].x,
        coordinates[i].y,
        pixels[i].a,
        pixels[i].b,
        pixels[i].c);
    }
  }

  // drawing methods for the color classes
  inline void drawCircleToImage(
    ColorClasses::Color color, 
    const int startX,
    const int startY,
    const int radius)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawCircleToImage(*this, startX, startY, radius, yy, cb, cr);
  }

  inline void drawRectToImage(
    ColorClasses::Color color, 
    const int startX,
    const int startY,
    const int endX,
    const int endY)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawRectToImage(*this, startX, startY, endX, endY, yy, cb, cr);
  }

  inline void drawLineToImage(
    ColorClasses::Color color, 
    const int startX,
    const int startY,
    const int endX,
    const int endY)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawLineToImage(*this, startX, startY, endX, endY, yy, cb, cr);
  }

  inline void drawPointToImage(
    ColorClasses::Color color, 
    const int x,
    const int y)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawPointToImage(*this, x, y, yy, cb, cr);
  }

  inline void drawPointToImage(
    const unsigned char yy,
    const unsigned char cb,
    const unsigned char cr, 
    const int x,
    const int y)
  {
    naoth::ImageDrawings::drawPointToImage(*this, x, y, yy, cb, cr);
  }

private:
  size_t numberOfPoints; // the number of points currently stored in the 'coordinates'
  Vector2i size; // the size of the canvas (x ~ width, y ~ height)
  std::vector<Pixel> pixels;
  std::vector<Vector2i> coordinates;
};

// representations: for top and the bottom image
class DebugImageDrawings : public DebugDrawingCanvas{};
class DebugImageDrawingsTop : public DebugImageDrawings{};


#ifdef DEBUG
#define CIRCLE_PX(...) getDebugImageDrawings().drawCircleToImage(__VA_ARGS__)
#define RECT_PX(...) getDebugImageDrawings().drawRectToImage(__VA_ARGS__)
#define LINE_PX(...) getDebugImageDrawings().drawLineToImage(__VA_ARGS__)
#define POINT_PX(...) getDebugImageDrawings().drawPointToImage(__VA_ARGS__)
#else
/* ((void)0) - that's a do-nothing statement */
#define CIRCLE_PX(color,x,y,radius) ((void)0)
#define RECT_PX(color,x0,y0,x1,y1) ((void)0)
#define LINE_PX(color,x0,y0,x1,y1) ((void)0)
#define POINT_PX(color,x,y) ((void)0)
#endif //DEBUG

#endif  /* _DebugImageDrawings_H */
