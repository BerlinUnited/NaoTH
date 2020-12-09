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
#include <Tools/Math/Common.h>
#include <cmath>


class KickActionModel:public naoth::Printable
{
public:

  KickActionModel()
    : bestAction(none)
  {}

  virtual ~KickActionModel() {}

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
  double getRotation();

  ActionId bestAction;
  //double rotation;
  Vector2d expectedBallPos;

  virtual void print(std::ostream& stream) const
  {
	  stream << "expectedBallPos.x = " << expectedBallPos.x << std::endl;
	  stream << "expectedBallPos.y = " << expectedBallPos.y << std::endl;
    stream << "bestAction = " << getName(bestAction) << std::endl;
    //stream << "rotation for bestAction = " << Math::toDegrees(rotation) << std::endl;
  }
};


#endif  /* _KickActionModel_H */

