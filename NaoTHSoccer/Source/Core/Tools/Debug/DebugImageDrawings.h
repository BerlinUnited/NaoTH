// 
// File:   DebugImageDrawings.h
// Author: Heinrich Mellmann
//
// Created on 19. März 2008, 21:51
//

#ifndef _DebugImageDrawings_H
#define _DebugImageDrawings_H

#include <Tools/DataStructures/Singleton.h>
#include <Tools/ImageProcessing/ImageDrawings.h>

#include <Tools/ImageProcessing/ImagePrimitives.h>
#include <Tools/Math/Vector2.h>
#include "Tools/ColorClasses.h"

#include <vector>

class DebugImageDrawings : public naoth::Singleton<DebugImageDrawings>, public naoth::DrawingCanvas
{
protected:
  friend class naoth::Singleton<DebugImageDrawings>;
  DebugImageDrawings();
  virtual ~DebugImageDrawings();

public:
  virtual void drawPoint(
    const unsigned int& x, 
    const unsigned int& y,
    const unsigned char& a,
    const unsigned char& b,
    const unsigned char& c);

  unsigned int width() const;
  unsigned int height() const;
  void reset();
  void drawToImage(DrawingCanvas& image);

    

  // drawing methods for the color classes
  inline void drawCircleToImage(
    ColorClasses::Color color, 
    const unsigned int& startX,
    const unsigned int& startY,
    const unsigned int& radius)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawCircleToImage(*this, startX, startY, radius, yy, cb, cr);
  }

  inline void drawRectToImage(
    ColorClasses::Color color, 
    const unsigned int& startX,
    const unsigned int& startY,
    const unsigned int& endX,
    const unsigned int& endY)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawRectToImage(*this, startX, startY, endX, endY, yy, cb, cr);
  }

  inline void drawLineToImage(
    ColorClasses::Color color, 
    const unsigned int& startX,
    const unsigned int& startY,
    const unsigned int& endX,
    const unsigned int& endY)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawLineToImage(*this, startX, startY, endX, endY, yy, cb, cr);
  }

  inline void drawPointToImage(
    ColorClasses::Color color, 
    const unsigned int& x,
    const unsigned int& y)
  {
    unsigned char yy, cb, cr;
    ColorClasses::colorClassToYCbCr(color, yy, cb, cr);
    naoth::ImageDrawings::drawPointToImage(*this, x, y, yy, cb, cr);
  }

private:
  std::vector<Pixel> pixels;
  std::vector<Vector2<unsigned int> > coordinates;
  int numberOfPoints;

};

#ifdef DEBUG
#define CIRCLE_PX(...) DebugImageDrawings::getInstance().drawCircleToImage(__VA_ARGS__)
#define RECT_PX(...) DebugImageDrawings::getInstance().drawRectToImage(__VA_ARGS__)
#define LINE_PX(...) DebugImageDrawings::getInstance().drawLineToImage(__VA_ARGS__)
#define POINT_PX(...) DebugImageDrawings::getInstance().drawPointToImage(__VA_ARGS__)
#else
/* ((void)0) - that's a do-nothing statement */
#define CIRCLE_PX(color,x,y,radius) ((void)0)
#define RECT_PX(color,x0,y0,x1,y1) ((void)0)
#define LINE_PX(color,x0,y0,x1,y1) ((void)0)
#define POINT_PX(color,x,y) ((void)0)
#endif //DEBUG

#endif  /* _DebugImageDrawings_H */
