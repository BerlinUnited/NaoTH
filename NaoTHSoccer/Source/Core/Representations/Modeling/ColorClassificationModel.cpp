#include "ColorClassificationModel.h"

ColorClassificationModel::ColorClassificationModel()
:
  fieldColorPerceptValid(false),
  baseColorRegionPerceptValid(false)
{
}

void ColorClassificationModel::setColorTable(const ColorTable64 &ct)
{
  colorTable = ct;
}

void ColorClassificationModel::invalidateFieldColorPercept()
{
  fieldColorPerceptValid = false;
}

void ColorClassificationModel::setFieldColorPercept(const FieldColorPercept& percept)
{
  fieldColorPercept = percept;
  fieldColorPerceptValid = true;
}

void ColorClassificationModel::invalidateBaseColorRegionPercept()
{
  baseColorRegionPerceptValid = false;
}

void ColorClassificationModel::setBaseColorRegionPercept(const BaseColorRegionPercept& percept)
{
  baseColorRegionPercept = percept;
  baseColorRegionPerceptValid = true;
}

void ColorClassificationModel::invalidateSimpleGoalColorPercept()
{
  simpleGoalColorPerceptValid = false;
}

void ColorClassificationModel::setSimpleGoalColorPercept(const SimpleGoalColorPercept& percept)
{
  simpleGoalColorPercept = percept;
  simpleGoalColorPerceptValid = true;
}

void ColorClassificationModel::invalidateSimpleBallColorPercept()
{
  simpleBallColorPerceptValid = false;
}

void ColorClassificationModel::setSimpleBallColorPercept(const SimpleBallColorPercept& percept)
{
  simpleBallColorPercept = percept;
  simpleBallColorPerceptValid = true;
}
