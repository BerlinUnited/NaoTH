/**
 * @file LinePercept.cpp
 * 
 * Definition of class LinePercept
 */ 

#include "LinePercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<LinePercept>::serialize(const LinePercept& representation, std::ostream& stream)
{
  
}//end serialize

void Serializer<LinePercept>::deserialize(std::istream& stream, LinePercept& representation)
{
  
}//end deserialize