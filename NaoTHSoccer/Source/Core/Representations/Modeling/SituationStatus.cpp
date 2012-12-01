/* 
 * File:   SituationStatus.h
 * Author: Paul & Michael
 *
 * Created on 01. März 2012
 */


#include "SituationStatus.h"


  /*
void SituationStatus::toDataStream(ostream& stream) const
{
  google::protobuf::io::OstreamOutputStreamLite buf(&stream);
  status.SerializePartialToZeroCopyStream(&buf);
}
  */

  /*
void SituationStatus::fromDataStream(istream& stream)
{
  status.Clear();
  google::protobuf::io::IstreamInputStreamLite buf(&stream);
  status.ParseFromZeroCopyStream(&buf);
}
  */

void SituationStatus::print(std::ostream& stream) const
{
  stream << "In own half = " << ownHalf << std::endl
         << "Reactive ball model requested = " << reactiveBallModelNeeded << std::endl;
}

SituationStatus::~SituationStatus() {
}
