/**
* @file GroundContactModel.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _GroundContactModel_h_
#define _GroundContactModel_h_

#include "Tools/DataStructures/Serializer.h"

class GroundContactModel
{
public:
  enum Foot {
    LEFT,
    RIGHT,
    NONE
  };

  GroundContactModel() : leftGroundContact(false), rightGroundContact(false) {}

  bool leftGroundContact;
  bool rightGroundContact;
  Foot supportFoot;
};

namespace naoth
{
  template<>
  class Serializer<GroundContactModel>
  {
  public:
    static void serialize(const GroundContactModel& representation,
                          std::ostream& stream);
    static void deserialize(std::istream& stream,
                            GroundContactModel& representation);
  };
}


#endif // _GroundContactModel_h_
