// 
// File:   DebugImageDrawings.h
// Author: Heinrich Mellmann
//
// Created on 19. März 2008, 21:51
//

#ifndef _DebugImageDrawings_H
#define	_DebugImageDrawings_H

#include "Tools/DataStructures/Singleton.h"
#include "Tools/ImageProcessing/ImageDrawings.h"


namespace naoth
{
  
class DebugImageDrawings : public Singleton<DebugImageDrawings>, public DrawingCanvas
{
protected:
  friend class Singleton<DebugImageDrawings>;
  DebugImageDrawings();
  virtual ~DebugImageDrawings();

public:
  virtual void drawPoint(
    unsigned int x, 
    unsigned int y,
    unsigned char a,
    unsigned char b,
    unsigned char c);

  virtual unsigned int width();
  virtual unsigned int height();
  void reset();
  void drawToImage(DrawingCanvas& image);

private:
  unsigned char pixels[320*240][3];
  unsigned int coordinates[320*240][2];
  int numberOfPoints;

};

}

#ifdef DEBUG
#define CIRCLE_PX(color,x,y,radius) naoth::ImageDrawings::drawCircleToImage(naoth::DebugImageDrawings::getInstance(),color,x,y,radius)
#define RECT_PX(color,x0,y0,x1,y1) naoth::ImageDrawings::drawRectToImage(naoth::DebugImageDrawings::getInstance(),color,x0,y0,x1,y1)
#define LINE_PX(color,x0,y0,x1,y1) naoth::ImageDrawings::drawLineToImage(naoth::DebugImageDrawings::getInstance(),color,x0,y0,x1,y1)
#define POINT_PX(color,x,y) naoth::ImageDrawings::drawPointToImage(naoth::DebugImageDrawings::getInstance(),color,x,y)
#else
/* ((void)0) - that's a do-nothing statement */
#define CIRCLE_PX(color,x,y,radius) ((void)0)
#define RECT_PX(color,x0,y0,x1,y1) ((void)0)
#define LINE_PX(color,x0,y0,x1,y1) ((void)0)
#define POINT_PX(color,x,y) ((void)0)
#endif //DEBUG

#endif	/* _DebugImageDrawings_H */

