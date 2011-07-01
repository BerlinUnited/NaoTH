/* 
 * File:   BehaviorStatus.cpp
 * Author: thomas
 * 
 * Created on 22. März 2010, 09:10
 */

#include "BehaviorStatus.h"

BehaviorStatus::BehaviorStatus() 
{
}

void BehaviorStatus::toDataStream(ostream& stream) const
{
  /*
  google::protobuf::io::OstreamOutputStreamLite buf(&stream);
  status.SerializePartialToZeroCopyStream(&buf);
  */
}

void BehaviorStatus::fromDataStream(istream& stream)
{
  /*
  status.Clear();
  google::protobuf::io::IstreamInputStreamLite buf(&stream);
  status.ParseFromZeroCopyStream(&buf);
  */
}

void BehaviorStatus::print(ostream& stream) const
{
  stream << "BehaviorStatus - no automatic output";
}

BehaviorStatus::~BehaviorStatus() {
}

