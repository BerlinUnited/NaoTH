/**
*/

#ifndef CalibrationRequest_H
#define CalibrationRequest_H

#include "Tools/DataStructures/Printable.h"

class CalibrationRequest: public naoth::Printable
{
public:
  CalibrationRequest(){}

  virtual void print(std::ostream& stream) const
  {
    stream << "performAutomaticCameraMatrixCalibration = " << performAutomaticCameraMatrixCalibration << std::endl;
  }

  bool performAutomaticCameraMatrixCalibration;
};


#endif  /* SOCCERSTRATEGY_H */

