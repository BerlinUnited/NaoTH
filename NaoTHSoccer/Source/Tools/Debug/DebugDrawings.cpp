// 
// File:   DebugDrawings.cpp
// Author: thomas
//
// Created on 19. march 2008, 21:52
//

#include "DebugDrawings.h"

void naoth::Serializer<DrawingCanvas2D>::serialize(const DrawingCanvas2D& object, std::ostream& stream)
{
  const std::string& buf = object.out().str();
  if (buf != "") {
    stream << buf;
  }
}

void naoth::Serializer<DrawingCanvas2D>::deserialize(std::istream& /*stream*/, DrawingCanvas2D& /*object*/)
{
}
