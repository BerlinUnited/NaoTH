
#include "TestModuleManager.h"

#include "Modules/ModuleA.h"
#include "Modules/ModuleB.h"

void TestModuleManager::init()
{
  ModuleCreator<ModuleA>* m = registerModule<ModuleA>("ModuleA");
  setModuleEnabled(std::string("ModuleA"), true);
  setModuleEnabled(std::string("ModuleA"), false);
  setModuleEnabled(std::string("ModuleA"), true);

  registerModule<ModuleB>("ModuleB");
  setModuleEnabled(std::string("ModuleB"), true);
  
  // FAIL test
  //registerModule<ModuleB>("ModuleA");
}//end init


void TestModuleManager::main()
{
  // execute all modules
  list<string>::const_iterator iter;
  for(iter = getExecutionList().begin(); iter != getExecutionList().end(); iter++)
  {
    // get entry
    AbstractModuleCreator* module = getModule(*iter);
    if(module != NULL && module->isEnabled())
    {
      cout << "-- begin --" << endl;

      list<Representation*>::const_iterator repr_iter = module->getModule()->getRequiredRepresentations().begin();
      for(;repr_iter != module->getModule()->getRequiredRepresentations().end(); repr_iter++)
      {
        cout << "use " << (*repr_iter)->getName() << endl;
      }//end for

      //cout << "executing module " << *iter << endl;
      module->execute();

      repr_iter = module->getModule()->getProvidedRepresentations().begin();
      for(;repr_iter != module->getModule()->getProvidedRepresentations().end(); repr_iter++)
      {
        cout << "provide " << (*repr_iter)->getName() << endl;;
      }//end for
      cout << "-- end --" << endl;
    }//end if
  }//end for
}//end main
