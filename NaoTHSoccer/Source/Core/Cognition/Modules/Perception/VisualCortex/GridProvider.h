/**
* @file GridProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class GridProvider
*/

#ifndef __GridProvider_h_
#define __GridProvider_h_

#include <ModuleFramework/Module.h>

// Debug
#include "Tools/Debug/DebugRequest.h"
//#include "Tools/Debug/DebugImageDrawings.h"
#include <Tools/Debug/Stopwatch.h>

//Tools
#include "Tools/ImageProcessing/ColoredGrid.h"
#include "Tools/ImageProcessing/Histogram.h"

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(GridProvider)
  REQUIRE(Image)
  REQUIRE(ColorTable64)

  PROVIDE(ColoredGrid)
  PROVIDE(Histogram)
END_DECLARE_MODULE(GridProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class GridProvider : public GridProviderBase
{
public:

  GridProvider();
  ~GridProvider(){}


  /** executes the module */
  void execute();

private:
  class SimpleColorClassifier: public ColorClassifier
{
public:

  SimpleColorClassifier() {}

  inline ColorClasses::Color getColorClass(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
  {
    return get(a, b, c);
  }

  inline ColorClasses::Color getColorClass(const Pixel& p) const
  {
    return get(p.a, p.b, p.c);
  }

  inline ColorClasses::Color get(const unsigned char& a, const unsigned char& b, const unsigned char& c) const
  {
    double d = (Math::sqr((255.0 - (double)b)) + Math::sqr((double)c)) / (2.0*255.0);
    unsigned char t = (unsigned char)Math::clamp(Math::round(d),0.0,255.0);

    if(t > 120)
      return ColorClasses::orange;
    else
      return ColorClasses::none;
  }//end get
};

  void calculateColoredGrid();

  const SimpleColorClassifier simpleColorClassifier;
/*
  const ColorClassifier& getColorTable64() const
  {
    return simpleColorClassifier;
  }
  */
};

#endif //__GridProvider_h_
