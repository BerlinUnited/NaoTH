/**
 * @file BallCandidates.cpp
 * 
 * Definition of class BallCandidates
 */ 

#include "BallCandidates.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<BallCandidates>::serialize(const BallCandidates& r, std::ostream& stream)
{
  naothmessages::BallCandidates p;

  for(BallCandidates::PatchesList::const_iterator i = r.patches.begin(); i != r.patches.end(); ++i) {
    naothmessages::BallCandidates_Patch* patch = p.add_patches();
    DataConversion::toMessage((*i).min, *(patch->mutable_min()));
    DataConversion::toMessage((*i).max, *(patch->mutable_max()));
    patch->set_data((*i).data.data(), (*i).data.size());
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<BallCandidates>::deserialize(std::istream& stream, BallCandidates& r)
{
  naothmessages::BallCandidates p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  
}//end deserialize