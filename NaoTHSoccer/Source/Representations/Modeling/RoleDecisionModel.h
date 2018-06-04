/**
* @file RoleDecisionModel.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef ROLEDECISIONMODEL_H
#define ROLEDECISIONMODEL_H

using namespace std;

#include <list>
#include <limits>

class RoleDecisionModel: public naoth::Printable
{
public:

  unsigned int firstStriker;
  unsigned int secondStriker;

  std::list<unsigned int> aliveRobots;
  std::list<unsigned int> deadRobots;

  bool wantsToBeStriker;

  RoleDecisionModel():
    firstStriker(std::numeric_limits<int>::max()),
    secondStriker(std::numeric_limits<int>::max()),
    wantsToBeStriker(false)
  {
  }

  virtual void print(std::ostream& stream) const {

    if (firstStriker != std::numeric_limits<int>::max()) {
      stream << "First Striker: " << "Robot No. " << firstStriker << "\n";
    }
    else {
      stream << "Currently no first striker in use.\n";
    }

    if (secondStriker != std::numeric_limits<int>::max()) {
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
    
    stream << "The following robots are considered ALIVE:\n";
    for (std::list<unsigned int>::const_iterator iter = aliveRobots.begin(); iter != aliveRobots.end(); iter++) {
      stream << "Robot " << *iter << std::endl;
    }
    stream << "\nThe following robots are considered DEAD:\n";    
    for (std::list<unsigned int>::const_iterator iter = deadRobots.begin(); iter != deadRobots.end(); iter++) {
      stream << "Robot " << *iter << std::endl;
    }

  }

  void resetRobotStates() {
      aliveRobots.clear();
      deadRobots.clear();
  }

  void resetStriker() {
      firstStriker = std::numeric_limits<int>::max();
      secondStriker = std::numeric_limits<int>::max();
  }
};


#endif  /* ROLEDECISIONMODEL_H */

