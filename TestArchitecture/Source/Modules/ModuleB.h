#ifndef _ModuleB_h_
#define _ModuleB_h_

#include "ModuleFramework/Representation.h"
#include "ModuleFramework/Module.h"

// representations
#include "Representations/RepresentationB.h"
#include "Representations/RepresentationA.h"

#include <list>
#include <cstring>


BEGIN_DECLARE_MODULE(ModuleB)
  REQUIRE(RepresentationA)
  REQUIRE(RepresentationB)
END_DECLARE_MODULE(ModuleB)


class ModuleB: protected ModuleBBase
{

public:

  ModuleB(){}

  virtual void execute();

};//end class ModuleB

#endif //__ModuleB_h_