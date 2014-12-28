#ifndef COLORCLASSIFICATIONMODEL_H
#define COLORCLASSIFICATIONMODEL_H

//#include "Tools/ImageProcessing/ColorClassifier.h"
#include <Representations/Infrastructure/ColorTable64.h>
#include <Representations/Perception/FieldColorPercept.h>

#include "Tools/Math/Common.h"
#include <Tools/DataStructures/Printable.h>
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugRequest.h"


using namespace naoth;


class SimpleColorClassifier
{
public:

  SimpleColorClassifier() 
  {
    //DEBUG_REQUEST_REGISTER("ColorClassificationModel:use_simpleColorClassifier", "", false);
  }

  inline ColorClasses::Color getColorClass(const Pixel& p) const
  {
    unsigned char b = p.b;
    unsigned char c = p.c;

    // ball color
      double d = (Math::sqr((255.0 - (double)b)) + Math::sqr((double)c)) / (2.0*255.0);
      unsigned char t = (unsigned char)Math::clamp(Math::round(d),0.0,255.0);

      double orange_thresh = 115;
      //MODIFY("ColorClassificationModel:orange_thresh", orange_thresh);
      if(t > orange_thresh)
      {
        return ColorClasses::orange;
      }

      double yb = c-b;

      double yellow_thresh = 40;
      //MODIFY("ColorClassificationModel:yellow_thresh", yellow_thresh);
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

  inline ColorClasses::Color getColorClass(const Pixel& p) const
  {
    // green
    if(fieldColorPerceptValid && fieldColorPercept.isFieldColor(p.y, p.u, p.v))
    {
      return ColorClasses::green;
    }//end if

    //// simple classifier (used for tests)
    //DEBUG_REQUEST("ColorClassificationModel:use_simpleColorClassifier",
    //  return simpleColorClassifier.getColorClass(p);
    //);

    // default fallback
    return colorTable.getColorClass(p.y, p.u, p.v);
  }//end getColorClass

  const FieldColorPercept& getFieldColorPercept() const
  {
    return fieldColorPercept;
  }

  FieldColorPercept& getFieldColorPercept()
  {
    return fieldColorPercept;
  }

  virtual void print(std::ostream& stream) const
  {
    stream << "fieldColorPerceptValid: " << fieldColorPerceptValid << std::endl;
  }

private:
  // static color table
  ColorTable64 colorTable;

  // field color detector
  bool fieldColorPerceptValid;
  FieldColorPercept fieldColorPercept;

  // ...a hacked classifier
  SimpleColorClassifier simpleColorClassifier;
};

class ColorClassificationModelTop : public ColorClassificationModel
{
public:
  virtual ~ColorClassificationModelTop() {}
};



#endif // COLORCLASSIFICATIONMODEL_H
