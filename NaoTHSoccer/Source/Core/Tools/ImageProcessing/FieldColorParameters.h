#ifndef FIELDCOLORPARAMETERS_H
#define FIELDCOLORPARAMETERS_H

#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include <Tools/ImageProcessing/ImagePrimitives.h>


class FieldColorParameters : public ParameterList
{
public:

  double CromaRedChannelDistance;
  int MaxCromaBlueChannelValue;
  int MaxBrightnessChannelValue;

  FieldColorParameters();
  ~FieldColorParameters();
};
#endif // FIELDCOLORPARAMETERS_H
