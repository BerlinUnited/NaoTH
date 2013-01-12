
#include "TestModuleManager.h"

#include "Modules/ModuleA.h"
#include "Modules/ModuleB.h"

using namespace std;

void TestModuleManager::init()
{
  ModuleCreator<ModuleA>* m = registerModule<ModuleA>("ModuleA");
  setModuleEnabled(std::string("ModuleA"), false);
  setModuleEnabled(std::string("ModuleA"), false);
  setModuleEnabled(std::string("ModuleA"), false);

  registerModule<ModuleB>("ModuleB");
  setModuleEnabled(std::string("ModuleB"), false);
  
  // FAIL test
  //registerModule<ModuleB>("ModuleA");
}//end init


void TestModuleManager::main()
{
  {
  cout << "-- begin static test --" << endl;
  list<string>::const_iterator iter;
  for(iter = getExecutionList().begin(); iter != getExecutionList().end(); iter++)
  {
    // get entry
    AbstractModuleCreator* module = getModule(*iter);
    if(module != NULL)
    {
      cout << "-- " << module->moduleClassName() << endl;

      RegistrationInterfaceMap::const_iterator repr_iter = module->staticRequired().begin();
      for(;repr_iter != module->staticRequired().end(); repr_iter++)
      {
        cout << "require " << repr_iter->first << endl;
      }//end for

      repr_iter = module->staticProvided().begin();
      for(;repr_iter != module->staticProvided().end(); repr_iter++)
      {
        cout << "provide " << repr_iter->first << endl;;
      }//end for
      cout << "-- " << endl;
    }//end if
  }//end for
  cout << "-- end static test --" << endl;
  }

  cout << endl;

  setModuleEnabled("ModuleB", true);
  setModuleEnabled("ModuleA", true);
  // execute all modules
  {
  cout << "-- begin execution test --" << endl;
  list<string>::const_iterator iter;
  for(iter = getExecutionList().begin(); iter != getExecutionList().end(); iter++)
  {
    // get entry
    AbstractModuleCreator* module = getModule(*iter);
    if(module != NULL && module->isEnabled())
    {
      cout << "-- begin execute " << module->getModule()->getName() << " --" << endl;

      RepresentationMap::const_iterator repr_iter = module->getModule()->getRequire().begin();
      for(;repr_iter != module->getModule()->getRequire().end(); repr_iter++)
      {
        cout << "require " << repr_iter->second->getName() << endl;
      }//end for

      //cout << "executing module " << *iter << endl;
      module->execute();

      repr_iter = module->getModule()->getProvide().begin();
      for(;repr_iter != module->getModule()->getProvide().end(); repr_iter++)
      {
        cout << "provide " << repr_iter->second->getName() << endl;;
      }//end for
      cout << "-- end execute " << module->getModule()->getName() << " --" << endl;
    }//end if
  }//end for
  cout << "-- end execution test --" << endl;
  }
}//end main
