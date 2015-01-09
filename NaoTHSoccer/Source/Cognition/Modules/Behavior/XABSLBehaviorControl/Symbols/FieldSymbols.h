#ifndef __FieldSymbols_H_
#define __FieldSymbols_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Infrastructure/FieldInfo.h"

BEGIN_DECLARE_MODULE(FieldSymbols)
  REQUIRE(FieldInfo)
END_DECLARE_MODULE(FieldSymbols)

class FieldSymbols: public FieldSymbolsBase
{

public:
  FieldSymbols()
    :
  fieldInfo(getFieldInfo())
  {
    theInstance = this;
  };
  virtual ~FieldSymbols(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static FieldSymbols* theInstance;

  


  // representations
  const FieldInfo& fieldInfo;

  static double xPosFirstDefenseline();
  static double xPosSecondDefenseline();
  static double yPosLeftFlank();
  static double yPosRightFlank();

  static double xPosFirstAttackline();
  static double xPosSecondAttackline();
  static double xPosOppPenaltyMark();
  static double xPosOwnPenaltyMark();

};//end class FieldSymbols

#endif // __FieldSymbols_H_
