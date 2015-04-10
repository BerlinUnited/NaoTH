/**
* @file RoleDecisionModel.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
*/

#ifndef ROLEDECISIONMODEL_H
#define ROLEDECISIONMODEL_H

using namespace std;


class RoleDecisionModel: public naoth::Printable
{
public:

  int firstStriker;
  int secondStriker;

  bool wantsToBeStriker;

  RoleDecisionModel():
    firstStriker(-1),
    secondStriker(-1),
    wantsToBeStriker(false)
  {
  }

  virtual void print(std::ostream& stream) const
  {
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
      stream << "Robot wants to be a striker in the next round.\n";
    }
    else {      
      stream << "Robot does not want to be a striker in the next round.\n";
    }
  }



};


#endif  /* ROLEDECISIONMODEL_H */

