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

void ColorClassificationModel::setFieldColorPercept(const FieldColorPercept &percept)
{
  fieldColorPercept = percept;
  fieldColorPerceptValid = true;
}

void ColorClassificationModel::invalidateBaseColorRegionPercept()
{
  fieldColorPerceptValid = false;
}

void ColorClassificationModel::setBaseColorRegionPercept(const BaseColorRegionPercept &percept)
{
  baseColorRegionPercept = percept;
  fieldColorPerceptValid = true;
}
