#include "Representations/Modeling/BodyState.h"

#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

 void Serializer<BodyState>::serialize(const BodyState& representation, std::ostream& stream)
 {
    naothmessages::BodyState message;

    message.set_isliftedup(representation.isLiftedUp);

    google::protobuf::io::OstreamOutputStream buf(&stream);
    message.SerializePartialToZeroCopyStream(&buf);
}

void Serializer<BodyState>::deserialize(std::istream& stream, BodyState& representation)
{
    naothmessages::BodyState message;
    google::protobuf::io::IstreamInputStream buf(&stream);
    message.ParseFromZeroCopyStream(&buf);

    representation.isLiftedUp = message.isliftedup();
}

