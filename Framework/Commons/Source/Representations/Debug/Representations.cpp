/* 
 * File:   Representations.cpp
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include "Representations.h"


#include "Messages/Messages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

void naoth::Serializer<Representations>::serialize(const Representations& object, std::ostream& stream)
{
  
}

void naoth::Serializer<Representations>::deserialize(std::istream& /*stream*/, Representations& /*object*/)
{
}
