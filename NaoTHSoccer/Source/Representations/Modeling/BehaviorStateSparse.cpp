/* 
 * File:   BehaviorStateSparse.cpp
 * Author: thomas
 * 
 * Created on 22. march 2010, 09:10
 */

#include "BehaviorStateSparse.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;

BehaviorStateSparse::BehaviorStateSparse()
{
}

void BehaviorStateSparse::print(ostream& stream) const
{
  stream << "BehaviorStateUpdate - no automatic output";
}

using namespace naoth;

void Serializer<BehaviorStateSparse>::serialize(const BehaviorStateSparse& representation, std::ostream& stream)
{
  google::protobuf::io::OstreamOutputStream buf(&stream);
  representation.state.SerializeToZeroCopyStream(&buf);
}

void Serializer<BehaviorStateSparse>::deserialize(std::istream& stream, BehaviorStateSparse& representation)
{
  google::protobuf::io::IstreamInputStream buf(&stream);
  if(!representation.state.ParseFromZeroCopyStream(&buf))
  {
    THROW("Serializer<BehaviorStateSparse>::deserialize failed");
  }
}

BehaviorStateSparse::~BehaviorStateSparse() {
}

