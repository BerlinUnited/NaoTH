#include "ColorClassificationModel.h"

ColorClassificationModel::ColorClassificationModel()
  : fieldColorPerceptValid(false)
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
