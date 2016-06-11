/* 
 * File:   SituationStatus.h
 * Author: Paul and Michael
 *
 * Created on 01. Mrz 2012
 */

#ifndef _SITUATIONSTATUS_H
#define _SITUATIONSTATUS_H

#include "Tools/DataStructures/Printable.h"


class SituationStatus : public naoth::Printable
{
public:
  SituationStatus()
  :
    ownHalf(false),
    oppHalf(false),
    reactiveBallModelNeeded(false)
  {
  }

  ~SituationStatus(){}

  bool ownHalf; //force selflocator to locate in own half (e.g. after penalized)
  bool oppHalf; //force selflocator to locate in opp half (e.g. for debugging attacksituations)
  bool reactiveBallModelNeeded; // NOTE: deprecated
  virtual void print(std::ostream& stream) const
  {
    stream << "In own half = " << ownHalf << std::endl;
  }

};
#endif  /* _SITUATIONSTATUS_H */

