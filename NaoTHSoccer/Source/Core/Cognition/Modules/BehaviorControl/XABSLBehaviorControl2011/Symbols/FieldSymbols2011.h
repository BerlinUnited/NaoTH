#ifndef __FieldSymbols2011_H_
#define __FieldSymbols2011_H_

#include <ModuleFramework/Module.h>
#include <XabslEngine/XabslEngine.h>

// representations
#include "Representations/Infrastructure/FieldInfo.h"

BEGIN_DECLARE_MODULE(FieldSymbols2011)
  REQUIRE(FieldInfo)
END_DECLARE_MODULE(FieldSymbols2011)

class FieldSymbols2011: public FieldSymbols2011Base
{

public:
  FieldSymbols2011()
    :
  fieldInfo(getFieldInfo())
  {
    theInstance = this;
  };
  ~FieldSymbols2011(){};
  
  /** registers the symbols at an engine */
  void registerSymbols(xabsl::Engine& engine);

  virtual void execute();
private:

  static FieldSymbols2011* theInstance;

  // representations
  const FieldInfo& fieldInfo;

};//end class FieldSymbols

#endif // __FieldSymbols2011_H_
