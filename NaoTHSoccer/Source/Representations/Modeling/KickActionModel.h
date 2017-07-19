/* 
* File:   KickActionModel.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class KickActionModel
*/

#ifndef _KickActionModel_H_
#define _KickActionModel_H_

#include <string>
#include "Tools/DataStructures/Printable.h"
#include <Tools/Math/Vector2.h>

class KickActionModel:public naoth::Printable
{
public:

  KickActionModel()
    : bestAction(none)
  {}

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

  virtual void print(std::ostream& stream) const
  {
	  stream << "expectedBallPos.x = " << expectedBallPos.x << std::endl;
	  stream << "expectedBallPos.y = " << expectedBallPos.y << std::endl;
  }
};


#endif  /* _KickActionModel_H */

