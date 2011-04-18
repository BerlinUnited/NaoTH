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
      // only include enabled modules
      if(it->second->isEnabled())
      {
        modulesTODO.insert(it->first);
      }
    }
    
    int oldRepresentationCount = -1;
    
    while(oldRepresentationCount < availableRepresentations.size() && modulesTODO.size() > 0)
    {
      
      oldRepresentationCount = availableRepresentations.size();
      
      // go trough all inactive modules and check if their required representations are available
      for(set<string>::iterator it=modulesTODO.begin(); it != modulesTODO.end(); it++)
      {
        Module* m =  getModule(*it)->getModule();
        const list<Representation*> used = m->getUsedRepresentations(); 
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
          moduleExecutionList.push_back(*it);
          modulesTODO.erase(*it);
          
          // add all provided representations of this module to our known set
          const list<Representation*> provided = m->getProvidedRepresentations(); 
          for(list<Representation*>::const_iterator itProv=provided.begin(); itProv != provided.end(); itProv++)
          {
            availableRepresentations.insert((*itProv)->getName());
          }
        }
        
      } // for each module in modulesTODO
    } // end while something changed
    
    // print execution list
    cout << "automatic module execution list" << endl;
    cout << "-------------------------------" << endl;
    for(list<string>::const_iterator itExec = moduleExecutionList.begin(); 
      itExec != moduleExecutionList.end(); itExec++
    )
    {
      cout << *itExec << endl;
    }
    cout << endl;
    // deactivate inactive modules
    for(set<string>::const_iterator itTODO = modulesTODO.begin(); itTODO != modulesTODO.end(); itTODO++)
    { 
      // output error
      cerr << "WARNING: module \"" << *itTODO << "\" deactivated due to missing dependencies [";
      
      const list<Representation*> used = getModule(*itTODO)->getModule()->getUsedRepresentations(); 
      for(list<Representation*>::const_iterator itUsed=used.begin(); itUsed != used.end(); itUsed++)
      {
        if(availableRepresentations.find((*itUsed)->getName()) == availableRepresentations.end())
        {
          cerr << *itUsed << " ";
        }
      }
      cerr << "]" << endl;
      
      // deactivate
      getModule(*itTODO)->setEnabled(false);
    }
  }


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