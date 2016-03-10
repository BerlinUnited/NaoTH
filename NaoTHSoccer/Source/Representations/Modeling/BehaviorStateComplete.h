/* 
 * File:   BehaviorStateComplete.h
 * Author: thomas
 *
 * Created on 22. march 2010, 09:10
 */

#ifndef _BEHAVIORSTATECOMPLETE_H
#define _BEHAVIORSTATECOMPLETE_H

#include "Tools/DataStructures/Printable.h"
#include <Tools/DataStructures/Serializer.h>

#include "Messages/Messages.pb.h"

/**
 * This is a wrapper Representation for naothmessages::BehaviorStateComplete in
 * order to make the naothmessages::BehaviorStateComplete loggable
 */
class BehaviorStateComplete : public naoth::Printable
{
public:
  BehaviorStateComplete();
  virtual ~BehaviorStateComplete();

  virtual void print(std::ostream& stream) const;

  // list of all symbols, options and enumerations of the current behavior
  naothmessages::BehaviorStateComplete state;

};

namespace naoth
{
  template<>
  class Serializer<BehaviorStateComplete>
  {
  public:
    static void serialize(const BehaviorStateComplete& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, BehaviorStateComplete& representation);
  };
}

#endif  /* _BEHAVIORSTATECOMPLETE_H */

