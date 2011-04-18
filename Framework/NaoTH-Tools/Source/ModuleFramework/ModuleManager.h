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

#include "Module.h"
#include "BlackBoard.h"
#include "ModuleClassWraper.h"

using namespace std;

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
  
  /**
   * Calculate the execution list automatically from the dependencies
   */
  void calculateExecutionList()
  {
    moduleExecutionList.clear();
    
    
    // init helper sets
    set<string> availableRepresentations;
    set<string> modulesTODO;
    for(map<string, AbstractModuleCreator* >::const_iterator it=moduleExecutionMap.begin(); 
      it != moduleExecutionMap.end(); it++)
    {
      // only include enabled modules, not actuator and sensor
      if(it->second->isEnabled() && it->first != getSensorModuleName() && it->first != getActuatorModuleName())
      {
        modulesTODO.insert(it->first);
      }
    }
    
    // add default sensor module to the beginning
    internalAddModuleToExecutionList(getSensorModuleName(), availableRepresentations);
    
    int oldRepresentationCount = -1;
      
    while(oldRepresentationCount < (int) availableRepresentations.size() && modulesTODO.size() > 0)
    { 
      oldRepresentationCount = (int) availableRepresentations.size();
      
      // go trough all inactive modules and check if their required representations are available
      set<string>::iterator it=modulesTODO.begin();
      while(it != modulesTODO.end())
      {
        AbstractModuleCreator* am = getModule(*it);
        if(am != NULL && am->getModule() != NULL)
        {
          Module* m =  am->getModule();
          const list<Representation*> used = m->getRequiredRepresentations(); 
          bool somethingMissing = false;
          for(list<Representation*>::const_iterator itUsed=used.begin(); itUsed != used.end(); itUsed++)
          {
            if(availableRepresentations.find((*itUsed)->getName()) == availableRepresentations.end())
            {
              somethingMissing = true;
              break;
            }
          }
          
          if(!somethingMissing)
          {
            // add this module to the execution list
            internalAddModuleToExecutionList(*it, availableRepresentations);
            modulesTODO.erase(it++);
          }
        } // if module not null   
        it++;
      } // for each module in modulesTODO
    } // end while something changed
    
    // add default actuator module to the end (even if it has unresolved dependencies)
    internalAddModuleToExecutionList(getActuatorModuleName(), availableRepresentations);
    
    
    // print execution list
    cout << "automatic module execution list" << endl;
    cout << "-------------------------------" << endl;
    for(list<string>::const_iterator itExec = moduleExecutionList.begin(); 
      itExec != moduleExecutionList.end(); itExec++
    )
    {
      cout << *itExec << endl;
    }
    cout << "-------------------------------" << endl;
    cout << endl;
    // deactivate inactive modules
    for(set<string>::const_iterator itTODO = modulesTODO.begin(); itTODO != modulesTODO.end(); itTODO++)
    { 
      // output error
      cout << "WARNING: module \"" << *itTODO << "\" deactivated due to missing dependencies [";
      
      const list<Representation*> used = getModule(*itTODO)->getModule()->getRequiredRepresentations(); 
      for(list<Representation*>::const_iterator itUsed=used.begin(); itUsed != used.end(); itUsed++)
      {
        if(availableRepresentations.find((*itUsed)->getName()) == availableRepresentations.end())
        {
          cout << (*itUsed)->getName() << " ";
        }
      }
      cout << "]" << endl;
      
      // deactivate
      getModule(*itTODO)->setEnabled(false);
    }
  }
  
  virtual string getSensorModuleName() { return "Sensor";}
  virtual string getActuatorModuleName() { return "Actuator";}

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
  
  void internalAddModuleToExecutionList(string name, set<string>& availableRepresentations)
  {
    AbstractModuleCreator* am = getModule(name);
    if(am != NULL && am->getModule() != NULL)
    {
      Module* m = am->getModule();
      moduleExecutionList.push_back(name);
      
      // add all provided representations of this module to our known set
      const list<Representation*> provided = m->getProvidedRepresentations(); 
      for(list<Representation*>::const_iterator itProv=provided.begin(); itProv != provided.end(); itProv++)
      {
        availableRepresentations.insert((*itProv)->getName());
      }
    }
  } // end internalAddModuleToExecutionList
  
};

#endif //__ModuleManager_h_