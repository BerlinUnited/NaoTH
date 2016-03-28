/* 
* File:   SituationPrior.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class SituationPrior
*/

#ifndef _SituationPrior_H
#define _SituationPrior_H

#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/SampleSet.h"

class SituationPrior
{
public:

  SituationPrior() {}

  virtual ~SituationPrior() {}
  
  SampleSet theSampleSet;
};


#endif  /* _KickActionModel_H */

