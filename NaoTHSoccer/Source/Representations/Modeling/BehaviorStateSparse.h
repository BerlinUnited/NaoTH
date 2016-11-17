/* 
 * File:   BehaviorStateSparse.h
 * Author: thomas
 *
 * Created on 22. march 2010, 09:10
 */

#ifndef _BEHAVIORSTATESPARSE_H
#define _BEHAVIORSTATESPARSE_H

#include "Tools/DataStructures/Printable.h"
#include <Tools/DataStructures/Serializer.h>

#include "Messages/Messages.pb.h"

/**
 * This is a wrapper Representation for naothmessages::BehaviorStateSparse in
 * order to make the naothmessages::BehaviorStateSparse loggable
 */
class BehaviorStateSparse : public naoth::Printable
{
public:
  BehaviorStateSparse();
  virtual ~BehaviorStateSparse();

  virtual void print(std::ostream& stream) const;
  naothmessages::BehaviorStateSparse state;

};

namespace naoth
{
  template<>
  class Serializer<BehaviorStateSparse>
  {
  public:
    static void serialize(const BehaviorStateSparse& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, BehaviorStateSparse& representation);
  };
}

#endif  /* _BEHAVIORSTATESPARSE_H */

