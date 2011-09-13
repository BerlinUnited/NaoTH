/* 
 * File:   BehaviorStatus.h
 * Author: thomas
 *
 * Created on 22. März 2010, 09:10
 */

#ifndef _BEHAVIORSTATUS_H
#define  _BEHAVIORSTATUS_H

//#include "PlatformInterface/PlatformInterchangeable.h"
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

  virtual void toDataStream(ostream& stream) const;
  virtual void fromDataStream(istream& stream);

  virtual void print(ostream& stream) const;

  naothmessages::BehaviorStatus status;

};
#endif  /* _BEHAVIORSTATUS_H */

