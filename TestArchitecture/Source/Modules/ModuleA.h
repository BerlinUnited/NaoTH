#ifndef _ModuleA_h_
#define _ModuleA_h_

#include "ModuleFramework/Representation.h"
#include "ModuleFramework/Module.h"
#include "ModuleFramework/ModuleManager.h"

// representations
#include "Representations/RepresentationA.h"
#include "Representations/RepresentationB.h"

BEGIN_DECLARE_MODULE(ModuleA)
  REQUIRE(RepresentationA)
  PROVIDE(RepresentationB)
END_DECLARE_MODULE(ModuleA)

class ModuleA: protected ModuleABase, private ModuleManager
{
public:
  ModuleA()
  {
  }

  virtual void execute();

};//end class ModuleA

#endif //__ModuleA_h_