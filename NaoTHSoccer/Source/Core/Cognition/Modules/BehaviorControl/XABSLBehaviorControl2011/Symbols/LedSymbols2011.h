/**
* @file LedSymbols.h
*
* @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
* Definition of class LedSymbols
*/

#ifndef __LedSymbols2011_H_
#define __LEdSymbols2011_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>


#include "Tools/Math/Common.h"
#include <math.h>


// representations
#include "Representations/Infrastructure/LEDRequest.h"

BEGIN_DECLARE_MODULE(LedSymbols2011)
  PROVIDE(BehaviorLEDRequest)
END_DECLARE_MODULE(LedSymbols2011)

class LedSymbols2011: public LedSymbols2011Base
{

public:
  LedSymbols2011()
    :
    ledRequest(getBehaviorLEDRequest())
  {
    theInstance = this;
    lastREar = 0.0;
    lastLEar = 0.0;
    lastREye = 0;
    lastLEye = 0;
  };
  ~LedSymbols2011(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static LedSymbols2011* theInstance;

  // representations
  LEDRequest& ledRequest;

  // helper
  void setEye(naoth::LEDData::MultiLEDID from, naoth::LEDData::MultiLEDID to, int value);

  // setter and getter

  static void setRightEar(double id);
  static void setLeftEar(double id);
  static void setRightEye(int value);
  static void setLeftEye(int value);

  double lastREar;
  double lastLEar;
  int lastREye;
  int lastLEye;

  static double getRightEar();
  static double getLeftEar();
  static int getRightEye();
  static int getLeftEye();

};//end class LedSymbols

#endif // __LedSymbols2011_H_
