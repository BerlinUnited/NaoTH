/* 
 * File:   BehaviorStatus.cpp
 * Author: thomas
 * 
 * Created on 22. march 2010, 09:10
 */

#include "BehaviorStatus.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;

BehaviorStatus::BehaviorStatus() 
{
}

void BehaviorStatus::print(ostream& stream) const
{
  stream << "BehaviorStatus - no automatic output";
}

using namespace naoth;

void Serializer<BehaviorStatus>::serialize(const BehaviorStatus& representation, std::ostream& stream)
{
  google::protobuf::io::OstreamOutputStream buf(&stream);
  representation.status.SerializeToZeroCopyStream(&buf);
}

void Serializer<BehaviorStatus>::deserialize(std::istream& stream, BehaviorStatus& representation)
{
  google::protobuf::io::IstreamInputStream buf(&stream);
  if(!representation.status.ParseFromZeroCopyStream(&buf))
  {
    THROW("Serializer<BehaviorStatus>::deserialize failed");
  }
}

BehaviorStatus::~BehaviorStatus() {
}

