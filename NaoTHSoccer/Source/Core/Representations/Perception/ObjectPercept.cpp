/**
 * @file ObjectPercept.cpp
 * 
 * Definition of class ObjectPercept
 */ 

#include "ObjectPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<ObjectPercept>::serialize(const ObjectPercept& representation, std::ostream& stream)
{
  
}//end serialize

void Serializer<ObjectPercept>::deserialize(std::istream& stream, ObjectPercept& representation)
{
  
}//end deserialize