#include "ColorClassificationModel.h"

ColorClassificationModel::ColorClassificationModel()
:
  fieldColorPerceptValid(false),
  fieldColorPercept(fieldDummy),
  baseColorRegionPerceptValid(false),
  baseColorRegionPercept(baseDummy)
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
