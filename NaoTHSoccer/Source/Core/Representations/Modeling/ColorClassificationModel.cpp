#include "ColorClassificationModel.h"

ColorClassificationModel::ColorClassificationModel()
:
  colorTable(colorTableDummy),
  fieldColorPerceptValid(false),
  fieldColorPercept(fieldColorPerceptDummy),
  baseColorRegionPerceptValid(false),
  baseColorRegionPercept(baseColorRegionPerceptDummy)
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

void ColorClassificationModel::validateFieldColorPercept()
{
  fieldColorPerceptValid = true;
}

void ColorClassificationModel::setFieldColorPercept(const FieldColorPercept &percept)
{
  fieldColorPercept = percept;
}

void ColorClassificationModel::invalidateBaseColorRegionPercept()
{
  baseColorRegionPerceptValid = false;
}

void ColorClassificationModel::validateBaseColorRegionPercept()
{
  baseColorRegionPerceptValid = true;
}

void ColorClassificationModel::setBaseColorRegionPercept(const BaseColorRegionPercept &percept)
{
  baseColorRegionPercept = percept;
}
