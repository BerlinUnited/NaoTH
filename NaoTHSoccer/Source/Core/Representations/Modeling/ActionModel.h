/* 
 * File:   ActionModel.h
 * Author:
 *
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
    ball_position,
    kick_short,
    kick_long,
    sidekick_left,
    sidekick_right,
    none
  };
  
  ActionId myAction;

  virtual void print(std::ostream& stream) const
  {
    stream << "ActionModel";
  }
};


#endif  /* _ActionModel_H */

