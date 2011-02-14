/**
* @file ModuleManager.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class ModuleManager (base class for ModuleManagers)
*/

#ifndef __ModuleManager_h_
#define __ModuleManager_h_

#include "Module.h"
#include "BlackBoard.h"
#include "ModuleClassWraper.h"

class ModuleManager: virtual public BlackBoardInterface
{
public:
  virtual ~ModuleManager()
  {
    map<string, AbstractModuleCreator* >::iterator iter;
    for(iter = moduleExecutionMap.begin(); iter != moduleExecutionMap.end(); iter++)
    {
      delete (iter->second);
    }//end for
  }//end destructor

protected:

  /**
   * register a module
   * registration of different instances of the same class 
   * with different names is alowed
   */
  template<class T>
  ModuleCreator<T>* registerModule(std::string name)
  {
    // module does not exist
    if(moduleExecutionMap.find(name) == moduleExecutionMap.end())
    {
      moduleExecutionList.push_back(name);
      moduleExecutionMap[name] = createModule<T>();
    }

    AbstractModuleCreator* module = moduleExecutionMap.find(name)->second;
    ModuleCreator<T>* typedModule = dynamic_cast<ModuleCreator<T>*>(module);

    // check the type
    if(typedModule == NULL)
    {
      std::cerr << "Module type mismatch: " << name << " is already registered as "
                // HACK: getModuleName doesn't necessary return the type of the module
                << module->getModule()->getModuleName() << ", but "
                << typeid(T).name() << " requested." << std::endl;
      assert(false);
    }//end if

    return typedModule;
  }//end registerModule


  /**
   * enables or disables the module 'moduleName'
   * if the module 'moduleName' doesn't exist
   * no action is done
   */
  void setModuleEnabled(string moduleName, bool value)
  {
    if(moduleExecutionMap.find(moduleName) != moduleExecutionMap.end())
    {
      moduleExecutionMap[moduleName]->setEnabled(value);
    }
  }//end setModuleEnabled

  /**
   *
   */
  AbstractModuleCreator* getModule(string name)
  {
    map<string, AbstractModuleCreator* >::iterator iter = moduleExecutionMap.find(name);
    if(iter != moduleExecutionMap.end())
    {
      return iter->second;
    }

    // TODO: assert?
    return NULL;
  }//end getModule


  const list<string>& getExecutionList()
  {
    return moduleExecutionList;
  }//end getExecutionList


private:
  template<class T>
  ModuleCreator<T>* createModule()
  {
    return new ModuleCreator<T>(getBlackBoard());
  }//end createModule


  /** */
  //BlackBoard theBlackBoard;

  /** store the mapping name->module */
  map<string, AbstractModuleCreator* > moduleExecutionMap;

  /** list of names of modules in the order of registration */
  list<string> moduleExecutionList;
};

#endif //__ModuleManager_h_