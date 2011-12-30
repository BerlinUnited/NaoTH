#ifndef COLORCLASSIFICATIONMODEL_H
#define COLORCLASSIFICATIONMODEL_H

#include "Tools/ImageProcessing/ColorClassifier.h"
#include <Representations/Infrastructure/ColorTable64.h>
#include <Representations/Perception/FieldColorPercept.h>

using namespace naoth;

class ColorClassificationModel : public ColorClassifier
{
public:
  ColorClassificationModel();

  virtual void setColorTable(const ColorTable64& ct);
  virtual void setFieldColorPercept(const FieldColorPercept& percept);
  virtual void invalidateFieldColorPercept();

protected:
  virtual ColorClasses::Color get(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
  {
    if(fieldColorPerceptValid && fieldColorPercept.isFieldColor(a, b, c))
    {
      return ColorClasses::green;
    }
    return colorTable.getColorClass(a,b,c);
  }
private :

  ColorTable64 colorTable;
  FieldColorPercept fieldColorPercept;
  bool fieldColorPerceptValid;
};

#endif // COLORCLASSIFICATIONMODEL_H
