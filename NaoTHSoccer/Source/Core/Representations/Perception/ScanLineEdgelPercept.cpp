/**
 * @file ScanLineEdgelPercept.cpp
 * 
 * Definition of class ScanLineEdgelPercept
 */ 

#include "ScanLineEdgelPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<ScanLineEdgelPercept>::serialize(const ScanLineEdgelPercept& representation, std::ostream& stream)
{
  
}//end serialize

void Serializer<ScanLineEdgelPercept>::deserialize(std::istream& stream, ScanLineEdgelPercept& representation)
{
  
}//end deserialize