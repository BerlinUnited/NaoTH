/* 
 * File:   SituationStatus.h
 * Author: Paul and Michael
 *
 * Created on 01. Mrz 2012
 */

#ifndef _SITUATIONSTATUS_H
#define _SITUATIONSTATUS_H

#include "Tools/DataStructures/Printable.h"
#include "Messages/Messages.pb.h"

/**
 * This is a wrapper Representation for naothmessages::BehaviorStatus in
 * order to make the naothmessages::BehaviorStatus loggable
 */
class SituationStatus : public naoth::Printable
{
public:
  //standardconstructor
  SituationStatus()
  :
  id(unknown),
  ownHalf(false)
  {
  }
  ~SituationStatus();

  enum StatusID
  {
    unknown,
    after_penalized,
    dribble,
    kick,
    kickoff,
	  numOfStatus
  };

  StatusID id;

  bool ownHalf;
  virtual void print(ostream& stream) const;

};
#endif  /* _SITUATIONSTATUS_H */

