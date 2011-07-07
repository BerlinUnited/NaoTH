/* 
 * File:   AttentionModel.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _AttentionModel_H
#define	_AttentionModel_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"


class AttentionModel : public naoth::Printable
{
public:

  AttentionModel() {};

  virtual ~AttentionModel() {};


  Vector2<double> mostInterestingPoint;


  virtual void print(ostream& stream) const
  {
   stream << "AttentionModel";
  }//end print
};


#endif	/* _AttentionModel_H */

