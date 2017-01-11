
#include "SituationPrior.h"

std::string SituationPrior::getName(PriorId id)
{
  switch(id)
  {
    case none: return "none";
    case firstReady: return "firstReady";
    case positionedInSet: return "positionedInSet";
    case goaliePenalizedInSet: return "goaliePenalizedInSet";
    case set: return "set";
    case playAfterPenalized: return "playAfterPenalized";
    case oppHalf: return "oppHalf";
    default: return "unknown prior";
  }
}

void SituationPrior::print(std::ostream &stream) const
{
  stream << "currentPrior=" << getName(currentPrior) << std::endl;
}
