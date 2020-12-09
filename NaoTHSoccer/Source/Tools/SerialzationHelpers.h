/**
* @file SerialzationHelpers.h
* 
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Xu, Yuan</a>
* convert data to string or string to data
* or data to protobuf message
*/

#ifndef _SerialzationHelpers_H
#define _SerialzationHelpers_H

#include <Tools/DataConversion.h>
#include "Tools/ImageProcessing/Edgel.h"

namespace SerialzationHelpers
{
  // Edgel
  template<typename I, class O>
  void toMessage(const EdgelT<I>& edgel, O& msg)
  {
    naoth::DataConversion::toMessage(edgel.point, *msg.mutable_point());
    naoth::DataConversion::toMessage(edgel.direction, *msg.mutable_direction());
  }

  template<typename I, class O>
  void fromMessage(const O& msg, EdgelT<I>& edgel)
  {
    naoth::DataConversion::fromMessage(msg.point(), edgel.point);
    naoth::DataConversion::fromMessage(msg.direction(), edgel.direction);
  }

  // vectors
  template<class I, class O>
  void toMessage(const std::vector<I>& data, O& msg)
  {
    for (const I& v : data) {
      toMessage(v, *msg.Add());
    }
  }

  template<class I, class O>
  void fromMessage(const O& msg, std::vector<I>& data)
  {
    data.resize(msg.size());
    for (int i = 0; i < msg.size(); ++i) {
      fromMessage(msg.Get(i), data[i]);
    }
  }

} // namespace SerialzationHelpers

#endif  /* _SerialzationHelpers_H */


