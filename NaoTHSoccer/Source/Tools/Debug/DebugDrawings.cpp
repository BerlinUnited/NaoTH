// 
// File:   DebugDrawings.cpp
// Author: thomas
//
// Created on 19. march 2008, 21:52
//

#include "DebugDrawings.h"

void naoth::Serializer<DebugDrawings>::serialize(const DebugDrawings& object, std::ostream& stream)
{
  const std::string& buf = object.out().str();
  if (buf != "") {
    stream << buf;
  }
}

void naoth::Serializer<DebugDrawings>::deserialize(std::istream& /*stream*/, DebugDrawings& /*object*/)
{
}
