/**
* @file RoleDecisionModel.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef ROLEDECISIONMODEL_H
#define ROLEDECISIONMODEL_H

#include <limits>
#include <ostream>

#include "Tools/DataStructures/Printable.h"

class RoleDecisionModel: public naoth::Printable
{
public:

  unsigned int firstStriker;
  unsigned int secondStriker;

  bool wantsToBeStriker;

  RoleDecisionModel():
    firstStriker(std::numeric_limits<unsigned int>::max()),
    secondStriker(std::numeric_limits<unsigned int>::max()),
    wantsToBeStriker(false)
  {
  }

  virtual void print(std::ostream& stream) const {

    if (firstStriker != std::numeric_limits<unsigned int>::max()) {
      stream << "First Striker: " << "Robot No. " << firstStriker << "\n";
    }
    else {
      stream << "Currently no first striker in use.\n";
    }

    if (secondStriker != std::numeric_limits<unsigned int>::max()) {
      stream << "Second Striker: " << "Robot No. " << secondStriker << "\n";
    }
    else {
      stream << "Currently no second striker in use.\n\n";
    }

    if (wantsToBeStriker) {
      stream << "Robot wants to be a striker in the next round.\n\n";
    }
    else {      
      stream << "Robot does not want to be a striker in the next round.\n\n";
    }
  }

  void resetStriker() {
      firstStriker = std::numeric_limits<unsigned int>::max();
      secondStriker = std::numeric_limits<unsigned int>::max();
  }
};


#endif  /* ROLEDECISIONMODEL_H */

