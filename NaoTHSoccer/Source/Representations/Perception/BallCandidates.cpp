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
    patch->set_type(naothmessages::BallCandidates_Patch_Type_Y);
    DataConversion::toMessage((*i).min, *(patch->mutable_min()));
    DataConversion::toMessage((*i).max, *(patch->mutable_max()));
    patch->set_data((*i).data.data(), (*i).data.size());
  }

  for(BallCandidates::PatchesYUVList::const_iterator i = r.patchesYUV.begin(); i != r.patchesYUV.end(); ++i) {
    naothmessages::BallCandidates_Patch* patch = p.add_patches();
    patch->set_type(naothmessages::BallCandidates_Patch_Type_YUV);
    DataConversion::toMessage((*i).min, *(patch->mutable_min()));
    DataConversion::toMessage((*i).max, *(patch->mutable_max()));
    // 
    patch->set_data((*i).data.data(), (*i).data.size()*sizeof(Pixel));
  }

  for(BallCandidates::PatchYUVClassifiedList::const_iterator i = r.patchesYUVClassified.begin(); i != r.patchesYUVClassified.end(); ++i) {
    naothmessages::BallCandidates_Patch* patch = p.add_patches();
    patch->set_type(naothmessages::BallCandidates_Patch_Type_YUVC);
    DataConversion::toMessage((*i).min, *(patch->mutable_min()));
    DataConversion::toMessage((*i).max, *(patch->mutable_max()));
    // 
    patch->set_data((*i).data.data(), (*i).data.size()*sizeof(BallCandidates::ClassifiedPixel));
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  p.SerializeToZeroCopyStream(&buf);
}//end serialize


void Serializer<BallCandidates>::deserialize(std::istream& stream, BallCandidates& r)
{
  naothmessages::BallCandidates p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  r.reset();
  for(int i = 0; i < p.patches_size(); ++i) 
  {
    if(p.patches(i).type() == naothmessages::BallCandidates_Patch_Type_Y) {
      BallCandidates::Patch& patch = r.nextFreePatch();
      DataConversion::fromMessage(p.patches(i).min(), patch.min);
      DataConversion::fromMessage(p.patches(i).max(), patch.max);
      patch.data.resize(p.patches(i).data().size());
      memcpy(patch.data.data(), p.patches(i).data().data(), p.patches(i).data().size());
    } 
    else if(p.patches(i).type() == naothmessages::BallCandidates_Patch_Type_YUV) 
    {
      BallCandidates::PatchYUV& patch = r.nextFreePatchYUV();
      DataConversion::fromMessage(p.patches(i).min(), patch.min);
      DataConversion::fromMessage(p.patches(i).max(), patch.max);
//      ASSERT(p.patches(i).data().size() == patch.SIZE*patch.SIZE*sizeof(Pixel));
      patch.data.resize(p.patches(i).data().size()/sizeof(Pixel));
      memcpy(patch.data.data(), p.patches(i).data().data(), p.patches(i).data().size());
    } 
    else if(p.patches(i).type() == naothmessages::BallCandidates_Patch_Type_YUVC) 
    {
      BallCandidates::PatchYUVClassified& patch = r.nextFreePatchYUVClassified();
      DataConversion::fromMessage(p.patches(i).min(), patch.min);
      DataConversion::fromMessage(p.patches(i).max(), patch.max);
//      ASSERT(p.patches(i).data().size() == patch.SIZE*patch.SIZE*sizeof(BallCandidates::ClassifiedPixel));
      patch.data.resize(p.patches(i).data().size()/sizeof(BallCandidates::ClassifiedPixel));
      memcpy(patch.data.data(), p.patches(i).data().data(), p.patches(i).data().size());
    } else {
      ASSERT(false);
    }
  }

}//end deserialize
