#ifndef FIELDCOLORPARAMETERS_H
#define FIELDCOLORPARAMETERS_H
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/Debug/DebugParameterList.h"
#include <Tools/ImageProcessing/ImagePrimitives.h>

#define MIN_FIELD_COLOR_Y_CHANNEL_DIST 20
#define MIN_FIELD_COLOR_CB_CHANNEL_DIST 5
#define MIN_FIELD_COLOR_CR_CHANNEL_DIST 5
#define MAX_FIELD_COLOR_Y_CHANNEL_DIST 72
#define MAX_FIELD_COLOR_CB_CHANNEL_DIST 32
#define MAX_FIELD_COLOR_CR_CHANNEL_DIST 24

#define MIN_FIELD_COLOR_Y_LEVEL 5
#define MIN_FIELD_COLOR_CB_LEVEL 2
#define MIN_FIELD_COLOR_CR_LEVEL 2

#define MAX_FIELD_COLOR_Y_LEVEL 240
#define MAX_FIELD_COLOR_CB_LEVEL 255
#define MAX_FIELD_COLOR_CR_LEVEL 130

class FieldColorParameters : public ParameterList
{
public:
  DoublePixel fieldcolorDistMin;
  DoublePixel fieldcolorDistMax;

  DoublePixel fieldColorMin;
  DoublePixel fieldColorMax;

  FieldColorParameters();
  ~FieldColorParameters();
};
#endif // FIELDCOLORPARAMETERS_H
