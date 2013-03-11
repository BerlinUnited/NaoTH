/* 
 * File:   BehaviorStatus.h
 * Author: thomas
 *
 * Created on 22. march 2010, 09:10
 */

#ifndef _BEHAVIORSTATUS_H
#define _BEHAVIORSTATUS_H

#include "Tools/DataStructures/Printable.h"


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

  virtual void toDataStream(std::ostream& stream) const;
  virtual void fromDataStream(std::istream& stream);

  virtual void print(std::ostream& stream) const;

  naothmessages::BehaviorStatus status;

};
#endif  /* _BEHAVIORSTATUS_H */

