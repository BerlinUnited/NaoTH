#ifndef COLORCLASSIFICATIONMODEL_H
#define COLORCLASSIFICATIONMODEL_H

//#include "Tools/ImageProcessing/ColorClassifier.h"
#include <Representations/Infrastructure/ColorTable64.h>
#include <Representations/Perception/FieldColorPercept.h>
#include <Representations/Perception/BaseColorRegionPercept.h>

#include "Tools/Math/Common.h"
#include <Tools/DataStructures/Printable.h>
#include "Tools/Debug/DebugModify.h"

using namespace naoth;


class SimpleColorClassifier
{
public:

  SimpleColorClassifier() {}

  inline ColorClasses::Color getColorClass(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
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

      return ColorClasses::none;
  }//end get
};



class ColorClassificationModel: public naoth::Printable
{
public:
  ColorClassificationModel();

  // some setters for the provider
  void setColorTable(const ColorTable64& ct);
  void setFieldColorPercept(const FieldColorPercept& percept);
  void invalidateFieldColorPercept();
  void setBaseColorRegionPercept(const BaseColorRegionPercept& percept);
  void invalidateBaseColorRegionPercept();


  inline ColorClasses::Color getColorClass(const Pixel& p) const
  {
    return getColorClass(p.a, p.b, p.c);
  }

  inline ColorClasses::Color getColorClass(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
  {
    // ball green
    if(fieldColorPerceptValid && fieldColorPercept.isFieldColor(a, b, c))
    {
      return ColorClasses::green;
    }//end if

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
    }//end if
    
    // simple classifier (used for tests)
    //return simpleColorClassifier.getColorClass(a,b,c);
    
    // default fallback
    return colorTable.getColorClass(a,b,c);
  }//end getColorClass


  virtual void print(ostream& stream) const
  {
    stream << "fieldColorPerceptValid: " << fieldColorPerceptValid << endl;
    stream << "baseColorRegionPerceptValid: " << baseColorRegionPerceptValid << endl;
  }

private:
  // static color table
  ColorTable64 colorTable;

  // field color detector
  bool fieldColorPerceptValid;
  FieldColorPercept fieldColorPercept;

  // detector for the other colors
  bool baseColorRegionPerceptValid;
  BaseColorRegionPercept baseColorRegionPercept;


  // ...a hacked classifier
  SimpleColorClassifier simpleColorClassifier;
};

#endif // COLORCLASSIFICATIONMODEL_H
