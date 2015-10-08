#include "AllBallPercepts.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void AllBallPercepts::print(std::ostream &stream) const {
    for(size_t i = 0; i < allBallPercepts.size(); i++) {
        stream << "BallPercept Number " << i+1 << ":" << std::endl;
        allBallPercepts[i].print(stream);
        stream << "----------------------" << std::endl;
    }
}
