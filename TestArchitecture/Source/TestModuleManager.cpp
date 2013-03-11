
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
  cout << "-- begin static test --" << endl;
  ModuleManager::print(cout);
  cout << "-- end static test --" << endl;

  cout << endl;

  setModuleEnabled("ModuleB", true);
  setModuleEnabled("ModuleA", true);
  
  cout << "-- begin static test 2 --" << endl;
  ModuleManager::print(cout);
  cout << "-- end static test 2 --" << endl;
  
  cout << "-- begin dynamic test --" << endl;
  ModuleManager::print(cout);
  cout << "-- end dynamic test --" << endl;

  cout << endl;

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
      module->execute();
    }//end if
  }//end for
  cout << "-- end execution test --" << endl;
  }
}//end main
