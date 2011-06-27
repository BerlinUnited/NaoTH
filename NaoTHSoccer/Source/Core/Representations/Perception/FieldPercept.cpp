/**
 * @file FieldPercept.cpp
 * 
 * Definition of class FieldPercept
 */ 

#include "FieldPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<FieldPercept>::serialize(const FieldPercept& representation, std::ostream& stream)
{
  
}//end serialize

void Serializer<FieldPercept>::deserialize(std::istream& stream, FieldPercept& representation)
{
  
}//end deserialize