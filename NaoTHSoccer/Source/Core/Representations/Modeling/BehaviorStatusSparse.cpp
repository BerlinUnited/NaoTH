/* 
 * File:   BehaviorStatus.cpp
 * Author: thomas
 * 
 * Created on 22. march 2010, 09:10
 */

#include "BehaviorStatusSparse.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace std;

BehaviorStatusSparse::BehaviorStatusSparse()
{
}

void BehaviorStatusSparse::print(ostream& stream) const
{
  stream << "BehaviorStatusSparse - no automatic output";
}

using namespace naoth;

void Serializer<BehaviorStatusSparse>::serialize(const BehaviorStatusSparse& representation, std::ostream& stream)
{
  google::protobuf::io::OstreamOutputStream buf(&stream);
  representation.status.SerializeToZeroCopyStream(&buf);
}

void Serializer<BehaviorStatusSparse>::deserialize(std::istream& stream, BehaviorStatusSparse& representation)
{
  google::protobuf::io::IstreamInputStream buf(&stream);
  if(!representation.status.ParseFromZeroCopyStream(&buf))
  {
    THROW("Serializer<BehaviorStatusSparse>::deserialize failed");
  }
}

BehaviorStatusSparse::~BehaviorStatusSparse() {
}

