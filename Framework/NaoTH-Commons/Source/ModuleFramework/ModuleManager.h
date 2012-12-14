/**
* @file ModuleManager.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class ModuleManager (base class for ModuleManagers)
*/

#ifndef _ModuleManager_h_
#define _ModuleManager_h_

#include <map>
#include <set>
#include <string>
#include <list>
#include <iterator>
#include <vector>


#include "Module.h"
#include "BlackBoard.h"
#include "ModuleCreator.h"


class ModuleManager: virtual public BlackBoardInterface
{
public:
  virtual ~ModuleManager();

  /**
   * register a module
   * registration of different instances of the same class 
   * with different names is alowed
   */
  template<class T>
  ModuleCreator<T>* registerModule(std::string name, bool enable = false);


  /**
   * enables or disables the module 'moduleName'
   * if the module 'moduleName' doesn't exist
   * no action is done
   */
  void setModuleEnabled(std::string moduleName, bool value, bool recalculateExecutionList=false);

  /**
   * 
   */
  AbstractModuleCreator* getModule(const std::string& name);

  /**
   *
   */
  const AbstractModuleCreator* getModule(const std::string& name) const;


  const std::list<std::string>& getExecutionList() const
  {
    return moduleExecutionList;
  }//end getExecutionList  

private:
  template<class T>
  ModuleCreator<T>* createModule()
  {
    return new ModuleCreator<T>(getBlackBoard());
  }//end createModule

  /** store the mapping name->module */
  std::map<std::string, AbstractModuleCreator* > moduleExecutionMap;

  /** list of names of modules in the order of registration */
  std::list<std::string> moduleExecutionList;
  

protected:

  /** */
  void calculateExecutionList();


  /**
   * Calculate the execution list automatically from the dependencies
   */
  void calculateExecutionListOld();

private:

  /** */
  void internalAddModuleToExecutionList(
    std::string name, 
    std::set<std::string>& availableRepresentations, 
    std::map<std::string,std::string>& providerForRepresentation);
  
  /** */
  unsigned int countExecutionListErrors(const std::vector<std::string>& order, 
    std::map<std::string, std::list<std::string> >& required, 
    std::map<std::string, std::list<std::string> >& provided, bool outputError);
  
};


// TODO: move to ModuleManager.ipp
template<class T>
ModuleCreator<T>* ModuleManager::registerModule(std::string name, bool enable)
{
  // module does not exist, so create a new one
  if(moduleExecutionMap.find(name) == moduleExecutionMap.end())
  {
    moduleExecutionList.push_back(name);
    moduleExecutionMap[name] = createModule<T>();
  }
    
  // find the existing module
  AbstractModuleCreator* module = moduleExecutionMap.find(name)->second;
  ModuleCreator<T>* typedModule = dynamic_cast<ModuleCreator<T>*>(module);

  // ...and check its type
  if(typedModule == NULL)
  {
    std::cerr << "Module type mismatch: " << name << " is already registered as "
              // HACK: getModuleName doesn't necessary return the type of the module
              << module->moduleClassName() << ", but "
              << typeid(T).name() << " requested." << std::endl;
    assert(false);
  }//end if
    

  typedModule->setEnabled(enable);

  return typedModule;
}//end registerModule

#endif //_ModuleManager_h_
