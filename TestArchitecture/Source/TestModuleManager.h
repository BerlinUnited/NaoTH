#ifndef __TestModuleManager_h_
#define __TestModuleManager_h_

#include "ModuleFramework/ModuleManager.h"

class TestModuleManager: public ModuleManager
{
public:
  TestModuleManager(){ }
  ~TestModuleManager(){}

  void init();
  virtual void main();

};

#endif //__TestModuleManager_h_