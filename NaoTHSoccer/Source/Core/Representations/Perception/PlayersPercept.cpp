/**
 * @file PlayersPercept.cpp
 * 
 * Definition of class PlayersPercept
 */ 

#include "PlayersPercept.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<PlayersPercept>::serialize(const PlayersPercept& representation, std::ostream& stream)
{
  
}//end serialize

void Serializer<PlayersPercept>::deserialize(std::istream& stream, PlayersPercept& representation)
{
  
}//end deserialize