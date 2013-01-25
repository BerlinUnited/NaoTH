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

class ModuleManager: virtual public BlackBoardInterface, public Printable
{
public:
  virtual ~ModuleManager();

  /**
  * register a module
  * Add a module creator with type T and the given name to the registeredModules if there was none.
  * If a module with the given name and the same type exists in the map no new object is created.
  * In this case only the setEnabled is applied.
  * Multiple rigistration of modules with the same name but different types causes assertion.
  * @return a pointer to the typed module creator in the registeredModules
  */
  template<class T>
  ModuleCreator<T>* registerModule(std::string name, bool enabled = false);


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


  virtual void print(std::ostream& stream) const
  {
    std::list<std::string>::const_iterator iter;
    for(iter = getExecutionList().begin(); iter != getExecutionList().end(); ++iter)
    {
      getModule(*iter)->print(stream);
    }
  }//end print


private:
  /** creates a module creator based on the given type (for internal use only)*/
  template<class T>
  ModuleCreator<T>* createModule(bool enabled = false);

  /** store the mapping name->module of the registered modules */
  typedef std::map<std::string, AbstractModuleCreator* > ModuleCreatorMap;
  ModuleCreatorMap registeredModules;

  /** list of names of modules in the order of their registration */
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

// implementations for the template methods
#include "ModuleManager.hpp"

#endif //_ModuleManager_h_
