/* 
* File:   SituationPrior.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class SituationPrior
*/

#ifndef _SituationPrior_H
#define _SituationPrior_H

#include <string>

class SituationPrior
{
public:

  SituationPrior() {}

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
  
  PriorId currentPrior;
};


#endif  /* _SituationPrior_H */

