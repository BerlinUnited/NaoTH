/* 
 * File:   ActionNew.h
* @author <a href="mailto:schlottb@informatik.hu-berlin.de">Benjamin Schlotter</a>
* Declaration of class ActionNew
*/
#ifndef _ActionNew_H
#define _ActionNew_H
#include <Tools/Math/Vector2.h>
class ActionNew
{
public:

  ActionNew() {}

  virtual ~ActionNew() {}

  Vector2d impactVector;
  double distribution;
};


#endif  /* _ActionNew_H */