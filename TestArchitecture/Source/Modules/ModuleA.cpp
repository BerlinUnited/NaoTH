
#include "ModuleA.h"
#include "ModuleB.h"

using namespace std;

#define M_PRINT(x) std::cout << "[" << getName() << "] " << x << std::endl;


void ModuleA::execute()
{
  M_PRINT("start");
  
  // provide a value (used by internal ModuleB)
  getRepresentationB().value = 3;


  // register a submodule
  ModuleCreator<ModuleB>* m = registerModule<ModuleB>("ModuleB");
  m->setEnabled(true); // enable m
  m->execute(); // run m


  // provide another value
  getRepresentationB().value = 4;

  M_PRINT("end");
}//end execute



