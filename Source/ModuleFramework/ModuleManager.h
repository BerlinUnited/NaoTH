/**
* @file ModuleManager.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class ModuleManager (base class for ModuleManagers)
*/

#ifndef __ModuleManager_h_
#define __ModuleManager_h_

#include <map>
#include <set>
#include <string>
#include <list>
#include <iterator>
#include <vector>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <iterator> 
#include <algorithm>

#include "Module.h"
#include "BlackBoard.h"
#include "ModuleCreator.h"

namespace crf
{

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

  /**
   * register a module
   * registration of different instances of the same class 
   * with different names is alowed
   */
  template<class T>
  ModuleCreator<T>* registerModule(std::string name, bool enable = false)
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

    typedModule->setEnabled(enable);

    return typedModule;
  }//end registerModule

  const list<string>& getExecutionList() const
  {
    return moduleExecutionList;
  }//end getExecutionList

//protected:

  /**
   * enables or disables the module 'moduleName'
   * if the module 'moduleName' doesn't exist
   * no action is done
   */
  void setModuleEnabled(string moduleName, bool value, bool recalculateExecutionList=false)
  {
    if(moduleExecutionMap.find(moduleName) != moduleExecutionMap.end())
    {
      moduleExecutionMap[moduleName]->setEnabled(value);
      if(recalculateExecutionList)
      {
        calculateExecutionList();
      }
    }
  }//end setModuleEnabled

  /**
   *
   */
  AbstractModuleCreator* getModule(const string& name)
  {
    map<string, AbstractModuleCreator* >::iterator iter = moduleExecutionMap.find(name);
    if(iter != moduleExecutionMap.end())
    {
      return iter->second;
    }

    // TODO: assert?
    return NULL;
  }//end getModule

  /**
   *
   */
  const AbstractModuleCreator* getModule(const string& name) const
  {
    map<string, AbstractModuleCreator* >::const_iterator iter = moduleExecutionMap.find(name);
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
  
protected:

  /**
   * Calculate the execution list automatically from the dependencies
   */
  void calculateExecutionList();

  
  virtual string getSensorModuleName() { return "Sensor"; }
  virtual string getActuatorModuleName() { return "Actuator"; }

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
  
  void internalAddModuleToExecutionList(string name, set<string>& availableRepresentations, map<string,string>& providerForRepresentation)
  {
    AbstractModuleCreator* am = getModule(name);
    if(am != NULL && am->isEnabled() && am->getModule() != NULL)
    {
      bool addModuleToList = true;
      Module* m = am->getModule();
      
      // add all provided representations of this module to our known set
      const list<Representation*> provided = m->getProvidedRepresentations(); 
      for(list<Representation*>::const_iterator itProv=provided.begin(); itProv != provided.end(); itProv++)
      {
        string repName = (*itProv)->getName();
        if(availableRepresentations.find(repName) == availableRepresentations.end())
        { 
          availableRepresentations.insert(repName);
          providerForRepresentation[repName] = name;
        }
        else
        {
          cerr << "ERROR when calculating execution order: " << repName << " provided more than once "
            << "(" << name << " and " << providerForRepresentation[repName] << ")" << endl;
            addModuleToList = false;
        }
      } // end for all provided representations
      if(addModuleToList)
      {
        moduleExecutionList.push_back(name);        
      }
    }
  } // end internalAddModuleToExecutionList
  
  unsigned int countExecutionListErrors(const vector<string>& order, 
    map<string, list<string> >& required, map<string, list<string> >& provided, bool outputError)
  {
    unsigned int errors = 0;
    
    set<string> availableRepresentations;
    
    for(vector<string>::const_iterator it = order.begin(); it != order.end(); it++)
    {
      string module = *it;
      const list<string>& req = required[module];
      const list<string>& prov = provided[module];
      
      for(list<string>::const_iterator itReq = req.begin(); itReq != req.end(); itReq++)
      {
        if(availableRepresentations.find(*itReq) == availableRepresentations.end())
        {
          errors++;
          if(outputError)
          {
            cerr << "ERROR: no provider for " << *itReq << " in " << module << endl;
          }
        }
      }
      
      for(list<string>::const_iterator itProv = prov.begin(); itProv != prov.end(); itProv++)
      {
        availableRepresentations.insert(*itProv);
      }
    }
    
    return errors;
  }//end countExecutionListErrors
  
};

}//crf

#endif //__ModuleManager_h_
