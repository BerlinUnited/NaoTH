/**
* @file Path.h
*
* @author <a href="mailto:yasinovs@informatik.hu-berlin.de">Kirill Yasinovskiy</a>
* Declaration of class Path
*/

#ifndef PATH_H_
#define PATH_H_

#include "Tools/Math/Common.h"
#include "Tools/Math/Vector2.h"
#include "Tools/DataStructures/Printable.h"

class Path: public naoth::Printable
{
public:
  // default constructor
  Path(){};
  // default destructor
  ~Path(){};


  Vector2d nextPointToGo;
  Vector2d targetPoint;

  virtual void print(std::ostream& stream) const
  {
  }//end print

};

#endif // end PATH_H_