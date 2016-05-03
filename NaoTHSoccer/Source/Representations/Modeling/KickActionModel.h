/* 
* File:   KickActionModel.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class KickActionModel
*/

#ifndef _KickActionModel_H_
#define _KickActionModel_H_

#include <string>

class KickActionModel
{
public:

  KickActionModel() {}

  virtual ~KickActionModel() {}

  // position of the most interesting point in robot coords
  enum ActionId
  {
    none,
    kick_short,
    kick_long,
    sidekick_left,
    sidekick_right,
    numOfActions
  };

  static std::string getName(ActionId id);
  
  ActionId bestAction;
  Vector2d expectedBallPos;
};


#endif  /* _KickActionModel_H */

