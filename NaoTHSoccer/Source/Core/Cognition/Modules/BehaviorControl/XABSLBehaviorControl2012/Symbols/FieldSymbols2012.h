#ifndef __FieldSymbols2012_H_
#define __FieldSymbols2012_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Infrastructure/FieldInfo.h"

BEGIN_DECLARE_MODULE(FieldSymbols2012)
  REQUIRE(FieldInfo)
END_DECLARE_MODULE(FieldSymbols2012)

class FieldSymbols2012: public FieldSymbols2012Base
{

public:
  FieldSymbols2012()
    :
  fieldInfo(getFieldInfo())
  {
    theInstance = this;
  };
  virtual ~FieldSymbols2012(){}
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static FieldSymbols2012* theInstance;

  // representations
  const FieldInfo& fieldInfo;

};//end class FieldSymbols2012

#endif // __FieldSymbols2012_H_
