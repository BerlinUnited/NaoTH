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
#include "Tools/DataStructures/Printable.h"

#include "Representations/Debug/Modules.h"

class ModuleManager: virtual public BlackBoardInterface, public Printable
{
public:
  virtual ~ModuleManager()
  {
    std::list<std::string>::const_iterator iter;
    for(iter = getExecutionList().begin(); iter != getExecutionList().end(); ++iter) {
      getModule(*iter)->setEnabled(false);
    }
  }

  /**
  * register a module
  * Add a module creator with type T and the given name to the registeredModules if there was none.
  * If a module with the given name and the same type exists in the map no new object is created.
  * In this case only the setEnabled is applied.
  * Multiple rigistration of modules with the same name but different types causes assertion.
  * @return a pointer to the typed module creator in the registeredModules
  */
  template<class T>
  ModuleCreator<T>* registerModule(const std::string& name, bool enabled = false) {
    ModuleCreator<T>* typedModule = getModules().registerModule<T>(name, enabled);
    moduleExecutionList.push_back(name);
    moduleModuleExecutionList.push_back(typedModule);
    return typedModule;
  }


  /**
  * enables or disables the module 'moduleName'
  * if the module 'moduleName' doesn't exist
  * no action is done
  */
  void setModuleEnabled(const std::string& name, bool value, bool recalculateExecutionList=false)
  {
    AbstractModuleCreator* module = getModule(name);

    if(module) {
      module->setEnabled(value);
    
      if(recalculateExecutionList) {
        calculateExecutionList();
      }
    }
  }

  /**
  * 
  */
  AbstractModuleCreator* getModule(const std::string& name) {
    return getModules().getModule(name);
  }

  /**
  *
  */
  const AbstractModuleCreator* getModule(const std::string& name) const {
    return getModules().getModule(name);
  }


  const std::list<std::string>& getExecutionList() const {
    return moduleExecutionList;
  }

  const std::list<AbstractModuleCreator*>& getModuleExecutionList() const {
    return moduleModuleExecutionList;
  }


  virtual void print(std::ostream& stream) const
  {
    std::list<std::string>::const_iterator iter;
    for(iter = getExecutionList().begin(); iter != getExecutionList().end(); ++iter) {
      getModule(*iter)->print(stream);
    }
  }//end print


private:
  /** list of names of modules in the order of their registration */
  std::list<std::string> moduleExecutionList;
  std::list<AbstractModuleCreator*> moduleModuleExecutionList;

protected:
  
  /** */
  void calculateExecutionList();


  /**
   * Calculate the execution list automatically from the dependencies
   */
  void calculateExecutionListOld();


private:

  const Modules& getModules() const {
    return getBlackBoard().getRepresentation<DataHolder<Modules> >("Modules");
  }

  Modules& getModules() {
    return getBlackBoard().getRepresentation<DataHolder<Modules> >("Modules");
  }

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

#endif //_ModuleManager_h_
