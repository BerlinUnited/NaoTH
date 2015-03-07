/* 
 * File:   Action.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class Action
*/
#ifndef _Action_H
#define _Action_H
#include <Tools/Math/Vector2.h>
class Action
{
public:

  Action() {}

  virtual ~Action() {}

  Vector2d impactVector;
  double distribution;
};


#endif  /* _Action_H */