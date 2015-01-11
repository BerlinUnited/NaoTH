/* 
 * File:   AttentionModel.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _AttentionModel_H
#define _AttentionModel_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"


class AttentionModel : public naoth::Printable
{
public:

  AttentionModel() {}

  virtual ~AttentionModel() {}

  // position of the most interesting point in robot coords
  Vector2<double> mostInterestingPoint;


  virtual void print(std::ostream& stream) const
  {
    stream << "AttentionModel";
  }//end print
};


#endif  /* _AttentionModel_H */

