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
  ~FieldSymbols(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static FieldSymbols* theInstance;

  // representations
  const FieldInfo& fieldInfo;

};//end class FieldSymbols

#endif // __FieldSymbols_H_
