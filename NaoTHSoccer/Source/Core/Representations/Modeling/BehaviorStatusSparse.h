/* 
 * File:   BehaviorStatus.h
 * Author: thomas
 *
 * Created on 22. march 2010, 09:10
 */

#ifndef _BEHAVIORSTATUSSPARSE_H
#define _BEHAVIORSTATUSSPARSE_H

#include "Tools/DataStructures/Printable.h"
#include <Tools/DataStructures/Serializer.h>

#include "Messages/Messages.pb.h"

/**
 * This is a wrapper Representation for naothmessages::BehaviorStatusSparse in
 * order to make the naothmessages::BehaviorStatusSparse loggable
 */
class BehaviorStatusSparse : public naoth::Printable
{
public:
  BehaviorStatusSparse();
  virtual ~BehaviorStatusSparse();

  virtual void print(std::ostream& stream) const;
  naothmessages::BehaviorStatusSparse status;

};

namespace naoth
{
  template<>
  class Serializer<BehaviorStatusSparse>
  {
  public:
    static void serialize(const BehaviorStatusSparse& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, BehaviorStatusSparse& representation);
  };
}

#endif  /* _BEHAVIORSTATUSSPARSE_H */

