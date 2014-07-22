/* 
 * File:   BehaviorStatus.cpp
 * Author: thomas
 * 
 * Created on 22. march 2010, 09:10
 */

#include "BehaviorStateComplete.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;

BehaviorStateComplete::BehaviorStateComplete()
{
}

void BehaviorStateComplete::print(ostream& stream) const
{
  stream << "BehaviorStateComplete - no automatic output";
}

using namespace naoth;

void Serializer<BehaviorStateComplete>::serialize(const BehaviorStateComplete& representation, std::ostream& stream)
{
  google::protobuf::io::OstreamOutputStream buf(&stream);
  representation.state.SerializeToZeroCopyStream(&buf);
}

void Serializer<BehaviorStateComplete>::deserialize(std::istream& stream, BehaviorStateComplete& representation)
{
  google::protobuf::io::IstreamInputStream buf(&stream);
  if(!representation.state.ParseFromZeroCopyStream(&buf))
  {
    THROW("Serializer<BehaviorStateComplete>::deserialize failed");
  }
}

BehaviorStateComplete::~BehaviorStateComplete() {
}

