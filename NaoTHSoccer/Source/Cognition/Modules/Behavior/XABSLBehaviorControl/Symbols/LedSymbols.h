/**
* @file LedSymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class LedSymbols
*/

#ifndef LED_SYMBOLS_H
#define LED_SYMBOLS_H

#include <array>

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>


// representations
#include "Representations/Infrastructure/LEDRequest.h"
#include "Tools/Debug/Color.h"

BEGIN_DECLARE_MODULE(LedSymbols)
  PROVIDE(BehaviorLEDRequest)
END_DECLARE_MODULE(LedSymbols)

class LedSymbols: public LedSymbolsBase
{

public:
  LedSymbols()
  {
    theInstance = this;
    lastREar = 0.0;
    lastLEar = 0.0;
    lastHead = 0.0;
    lastREye = 0;
    lastLEye = 0;
  };
  virtual ~LedSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();

private:

  static LedSymbols* theInstance;

  const std::array<Color, 4> colors = {{
    {0.0, 0.0, 0.0}, // off
    {1.0, 0.0, 0.0}, // red 
    {0.0, 1.0, 0.0}, // green
    {0.0, 0.0, 1.0}  // blue
  }};

  const Color& getEyeColor(int i) {
    ASSERT(0 <= i && i < colors.size());
    return colors[i];
  }

  // setter and getter

  static void setRightEar(double id);
  static void setLeftEar(double id);
  static void setHead(double id);
  static void setRightEye(int value);
  static void setLeftEye(int value);

  double lastREar;
  double lastLEar;
  double lastHead;
  int lastREye;
  int lastLEye;

  static double getRightEar();
  static double getLeftEar();
  static double getHead();
  static int getRightEye();
  static int getLeftEye();

};//end class LedSymbols

#endif // _LedSymbols_H_
