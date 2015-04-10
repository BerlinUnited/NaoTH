/**
* @file RoleDecisionModel.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef ROLEDECISIONMODEL_H
#define ROLEDECISIONMODEL_H

using namespace std;

#include <list>


class RoleDecisionModel: public naoth::Printable
{
public:

  int firstStriker;
  int secondStriker;

  std::list<int> aliveRobots;
  std::list<int> deadRobots;

  bool wantsToBeStriker;

  RoleDecisionModel():
    firstStriker(-1),
    secondStriker(-1),
    wantsToBeStriker(false)
  {
    aliveRobots = *(new std::list<int>());
    deadRobots = *(new std::list<int>());
  }

  virtual void print(std::ostream& stream) const {

    if (firstStriker != -1) {
      stream << "First Striker: " << "Robot No. " << firstStriker << "\n";
    }
    else {
      stream << "Currently no first striker in use.\n";
    }

    if (secondStriker != -1) {
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
    for (std::list<int>::const_iterator iter = aliveRobots.begin(); iter != aliveRobots.end(); iter++) {
      stream << "Robot " << *iter << std::endl;
    }
    stream << "\nThe following robots are considered DEAD:\n";    
    for (std::list<int>::const_iterator iter = deadRobots.begin(); iter != deadRobots.end(); iter++) {
      stream << "Robot " << *iter << std::endl;
    }

  }


};


#endif  /* ROLEDECISIONMODEL_H */

