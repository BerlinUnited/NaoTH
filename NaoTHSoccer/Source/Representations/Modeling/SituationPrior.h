/* 
* File:   SituationPrior.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Stella Alice Schlotter</a>
* Declaration of class SituationPrior
*/

#ifndef _SituationPrior_H
#define _SituationPrior_H

#include <string>
#include <Tools/DataStructures/Printable.h>

class SituationPrior : public naoth::Printable
{
public:

  SituationPrior() : currentPrior(none) {}

  virtual ~SituationPrior() {}
  
  enum PriorId
  {
    none,
    firstReady,
    positionedInSet,
    goaliePenalizedInSet,
    playAfterPenalized,
    oppHalf,
    set,
    numOfPriors
  };

  static std::string getName(PriorId id);

  virtual void print(std::ostream& stream) const;

  PriorId currentPrior;
};


#endif  /* _SituationPrior_H */

