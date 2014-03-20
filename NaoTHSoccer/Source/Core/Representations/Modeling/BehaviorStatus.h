/* 
 * File:   BehaviorStatus.h
 * Author: thomas
 *
 * Created on 22. march 2010, 09:10
 */

#ifndef _BEHAVIORSTATUS_H
#define _BEHAVIORSTATUS_H

#include "Tools/DataStructures/Printable.h"
#include <Tools/DataStructures/Serializer.h>

#include "Messages/Messages.pb.h"

/**
 * This is a wrapper Representation for naothmessages::BehaviorStatus in
 * order to make the naothmessages::BehaviorStatus loggable
 */
class BehaviorStatus : public naoth::Printable
{
public:
  BehaviorStatus();
  virtual ~BehaviorStatus();

  virtual void print(std::ostream& stream) const;

  naothmessages::BehaviorStatus status;

};

namespace naoth
{
  template<>
  class Serializer<BehaviorStatus>
  {
  public:
    static void serialize(const BehaviorStatus& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, BehaviorStatus& representation);
  };
}

#endif  /* _BEHAVIORSTATUS_H */

