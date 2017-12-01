/**
* @file LedSymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class LedSymbols
*/

#ifndef _LedSymbols_H_
#define _LedSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>


// representations
#include "Representations/Infrastructure/LEDRequest.h"

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

  // helper
  void setEye(naoth::LEDData::MultiLEDID from, naoth::LEDData::MultiLEDID to, int value);

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
