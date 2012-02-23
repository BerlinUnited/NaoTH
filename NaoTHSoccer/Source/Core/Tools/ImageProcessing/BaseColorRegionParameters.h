#ifndef BASECOLORREGIONPARAMETERS_H
#define BASECOLORREGIONPARAMETERS_H

#include <Tools/ImageProcessing/ImagePrimitives.h>
#include "Tools/Debug/DebugParameterList.h"

#define FIELD_IDX_Y 125.0
#define FIELD_IDX_CB 127.0
#define FIELD_IDX_CR 63.0

#define FIELD_DIST_Y 120.0
#define FIELD_DIST_CB 128.0
#define FIELD_DIST_CR 64.0

#define GOAL_IDX_Y 160.0
#define GOAL_IDX_CB 80.0
#define GOAL_IDX_CR 135.0

#define GOAL_DIST_Y 200.0
#define GOAL_DIST_CB 20.0
#define GOAL_DIST_CR 14.0

#define BALL_IDX_Y 140.0
#define BALL_IDX_CB 80.0
#define BALL_IDX_CR 200.0

#define BALL_DIST_Y 160.0
#define BALL_DIST_CB 96.0
#define BALL_DIST_CR 32.0

#define LINE_IDX_Y 254.0
#define LINE_IDX_CB 135.0
#define LINE_IDX_CR 125.0

#define LINE_DIST_Y 96.0
#define LINE_DIST_CB 8.0
#define LINE_DIST_CR 8.0

using namespace naoth;

class BaseColorRegionParameters : public ParameterList
{
public:
  DoublePixel fieldIndex;
  DoublePixel fieldDist;

  DoublePixel goalIndex;
  DoublePixel goalDist;

  DoublePixel ballIndex;
  DoublePixel ballDist;

  DoublePixel lineIndex;
  DoublePixel lineDist;

  BaseColorRegionParameters();
  ~BaseColorRegionParameters();
};


#endif // BASECOLORREGIONPARAMETERS_H
