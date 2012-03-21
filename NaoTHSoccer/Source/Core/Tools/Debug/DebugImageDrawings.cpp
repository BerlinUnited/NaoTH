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
  numberOfPoints(0)
{

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
    coordinates[numberOfPoints][0] = x;
    coordinates[numberOfPoints][1] = y;
    pixels[numberOfPoints][0] = a;
    pixels[numberOfPoints][1] = b;
    pixels[numberOfPoints][2] = c;
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
      coordinates[i][0],
      coordinates[i][1],
      pixels[i][0],
      pixels[i][1],
      pixels[i][2]);
  }//end for
}//end drawToImage

