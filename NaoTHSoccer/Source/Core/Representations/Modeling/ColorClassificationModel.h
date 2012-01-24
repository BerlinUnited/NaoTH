#ifndef COLORCLASSIFICATIONMODEL_H
#define COLORCLASSIFICATIONMODEL_H

#include "Tools/ImageProcessing/ColorClassifier.h"
#include <Representations/Infrastructure/ColorTable64.h>
#include <Representations/Perception/FieldColorPercept.h>
#include <Representations/Perception/BaseColorRegionPercept.h>

#include "Tools/Math/Common.h"


#include "Tools/Debug/DebugModify.h"

using namespace naoth;

class ColorClassificationModel : public ColorClassifier
{
public:
  ColorClassificationModel();

  virtual void setColorTable(const ColorTable64& ct);
  virtual void setFieldColorPercept(const FieldColorPercept& percept);
  virtual void invalidateFieldColorPercept();
  virtual void setBaseColorRegionPercept(const BaseColorRegionPercept& percept);
  virtual void invalidateBaseColorRegionPercept();

protected:
  virtual ColorClasses::Color get(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
  {
    // ball green
    if(fieldColorPerceptValid && fieldColorPercept.isFieldColor(a, b, c))
    {
      return ColorClasses::green;
    }

    if(baseColorRegionPerceptValid)
    {

      if(baseColorRegionPercept.isRedOrOrangeOrPink(a, b ,c))
      {
        return ColorClasses::orange;
      }
      else if(baseColorRegionPercept.isYellow(a, b ,c))
      {
        return ColorClasses::yellow;
      }
      else if(baseColorRegionPercept.isWhite(a, b ,c))
      {
        return ColorClasses::white;
      }
    }
    else
    {
      // ball color
      double d = (Math::sqr((255.0 - (double)b)) + Math::sqr((double)c)) / (2.0*255.0);
      unsigned char t = (unsigned char)Math::clamp(Math::round(d),0.0,255.0);

      double orange_thresh = 115;
      MODIFY("ColorClassificationModel:orange_thresh", orange_thresh);
      if(t > orange_thresh)
      {
        return ColorClasses::orange;
      }

      double yb = c-b;

      double yellow_thresh = 40;
      MODIFY("ColorClassificationModel:yellow_thresh", yellow_thresh);
      if(yb > yellow_thresh)
      {
        return ColorClasses::yellow;
      }
    }
    return colorTable.getColorClass(a,b,c);
  }
private :

  ColorTable64 colorTable;

  bool fieldColorPerceptValid;
  FieldColorPercept fieldDummy;
  FieldColorPercept& fieldColorPercept;

  bool baseColorRegionPerceptValid;
  BaseColorRegionPercept baseDummy;
  BaseColorRegionPercept& baseColorRegionPercept;
};

#endif // COLORCLASSIFICATIONMODEL_H
