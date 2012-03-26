/**
* @file CameraMatrixOffset.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _CameraMatrixOffset_h_
#define _CameraMatrixOffset_h_

#include "Tools/Math/Vector2.h"
#include "Tools/DataStructures/Printable.h"

class CameraMatrixOffset: public naoth::Printable
{
public:

  CameraMatrixOffset(){}
  ~CameraMatrixOffset(){}

  Vector2<double> offsetByGoalModel;
  Vector2<double> offset;

  virtual void print(ostream& stream) const
  {
    stream << "x = " << offset.x << endl;
    stream << "y = " << offset.y << endl;
  }//end print

};

#endif // _CameraMatrixOffset_h_
