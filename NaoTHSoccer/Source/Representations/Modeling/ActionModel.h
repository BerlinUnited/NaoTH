/* 
 * File:   ActionModel.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class ActionModel
*/

#ifndef _ActionModel_H
#define _ActionModel_H


class ActionModel : public naoth::Printable
{
public:

  ActionModel() {}

  virtual ~ActionModel() {}

  // position of the most interesting point in robot coords
  enum ActionId
  {
    none,
    kick_short,
    kick_long,
    sidekick_left,
    sidekick_right 
  };
  
  //number of ActionID's, has to write manual
  static const int numOfActions = 5;

  ActionId myAction;

  virtual void print(std::ostream& stream) const
  {
    stream << "ActionModel";
  }
};


#endif  /* _ActionModel_H */

