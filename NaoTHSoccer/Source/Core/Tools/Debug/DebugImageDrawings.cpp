// 
// File:   DebugImageDrawings.cpp
// Author: Heinrich Mellmann
//
// Created on 19. März 2008, 21:52
//

#include "DebugImageDrawings.h"

using namespace naoth;

DebugImageDrawings::DebugImageDrawings() 
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
}

DebugImageDrawings::~DebugImageDrawings(){}


void DebugImageDrawings::drawPoint(
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
}//end drawPoint

unsigned int DebugImageDrawings::width() const
{
  return 320;
}//end width

unsigned int DebugImageDrawings::height() const
{
  return 240;
}//end height

void DebugImageDrawings::reset()
{
  numberOfPoints = 0;
}//end reset

void DebugImageDrawings::drawToImage(DrawingCanvas& image)
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
}//end drawToImage

