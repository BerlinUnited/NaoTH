

#include "ModuleManager.h"

#include <ctime>

void ModuleManager::calculateExecutionList()
{
    
  bool changed = true;
  std::list<std::string>::iterator start=moduleExecutionList.begin();
  const int maxAttempts = static_cast<int> (moduleExecutionList.size())*10;
  int iterations = 0;

  while(changed && iterations < maxAttempts)
  {
    changed = false;
    iterations++;

    for(std::list<std::string>::iterator it1=start; it1 != moduleExecutionList.end(); ++it1)
    {
      start = it1;
      AbstractModuleCreator* mc1 = getModules().getModule(*it1);
      if(!mc1->isEnabled()) continue;
      Module* m1 = mc1->getModule();

      for(std::list<std::string>::iterator it2=it1; it2 != moduleExecutionList.end(); ++it2)
      {
        AbstractModuleCreator* mc2 = getModules().getModule(*it2);
        if(!mc2->isEnabled()) continue;
        Module* m2 = mc2->getModule();

        for(RepresentationMap::const_iterator itReq = m1->getRequire().begin();
          itReq != m1->getRequire().end(); ++itReq)
        {
          std::string repName = itReq->second->getName();
          for(RepresentationMap::const_iterator r = m2->getProvide().begin();
            r != m2->getProvide().end(); ++r)
          {
            if(r->second->getName() == repName)
            {
              std::swap(*it1, *it2);
              changed = true;
              break;
            }
          }//end for
          if(changed) break;
        }//end for
        if(changed) break;
      }//end for
      if(changed) break;
    }//end for

  }//end while

  if(iterations >= maxAttempts) {
    std::cerr << "WARNING: maximal number of iterations reached." << std::endl;
  }


  // print execution list
  std::cout << "automatic module execution list" << std::endl;
  std::cout << "-------------------------------" << std::endl;
  std::list<std::string>::const_iterator itExec = moduleExecutionList.begin();
  for(; itExec != moduleExecutionList.end(); ++itExec) {
    std::cout << *itExec << std::endl;
  }
  std::cout << "-------------------------------" << std::endl;
  std::cout << std::endl;
}//end calculateExecutionListNew


/**
  * Calculate the execution list automatically from the dependencies
  */
void ModuleManager::calculateExecutionListOld()
{
  std::map<std::string,std::string> providerForRepresentation;
  std::map<std::string, std::list<std::string> > required;
  std::map<std::string, std::list<std::string> > provided;
    
  // first fill the map with providers and the module graph
  //for(ModuleCreatorMap::const_iterator it = registeredModules.begin(); it != registeredModules.end(); ++it)
  for(std::list<std::string>::const_iterator name = moduleExecutionList.begin(); name != moduleExecutionList.end(); ++name)
  {
    AbstractModuleCreator* mc = getModules().getModule(*name);
    // only include enabled modules
    if(mc->isEnabled())
    {
      Module* m = mc->getModule();
        
      for(RepresentationMap::const_iterator itProv = m->getProvide().begin();
        itProv != m->getProvide().end(); ++itProv)
      {
        std::string repName = itProv->second->getName();
          
        if(providerForRepresentation.find(repName) == providerForRepresentation.end())
        {
          providerForRepresentation[repName] = *name;
          provided[*name].push_back(repName);
        }
        else
        {
          std::cerr << "ERROR: " << repName << " provided by both " << providerForRepresentation[repName]
            << " and " << *name;
        }
      }//end for
        
      for(RepresentationMap::const_iterator itReq = m->getRequire().begin();
        itReq != m->getRequire().end(); ++itReq)
      {
        std::string repName = itReq->second->getName();
        required[*name].push_back(repName);
      }//end for
        
    } // if module enabled
  }//end for
    
  // solve this constraint problem
  std::vector<std::string> oldExecutionList(moduleExecutionList.begin(), moduleExecutionList.end());
  unsigned int oldErrors = countExecutionListErrors(oldExecutionList, required, provided, false);
  unsigned int unsuccessfulAttempts = 0;
    
  const unsigned int maxAttempts = static_cast<unsigned int> (oldExecutionList.size())*10;
    
  // init to same seed to get "reliable" results
  srand(100);
    
  while(oldErrors != 0 && unsuccessfulAttempts < maxAttempts)
  {
    std::vector<std::string> newExecutionList = oldExecutionList;
    // switch randomly
    int left = 0;
    int right = 0;
      
    while(left == right)
    {
      left = rand() % static_cast<int> (newExecutionList.size());
      right = rand() % static_cast<int> (newExecutionList.size());
    }
      
    std::string buffer = newExecutionList[right];
    newExecutionList[right] = newExecutionList[left];
    newExecutionList[left] = buffer;
      
    unsigned int newErrors = countExecutionListErrors(newExecutionList, required, provided, false);
    //cerr << "new error = " << newErrors << " | old = " << oldErrors << " | no success = " << unsuccessfulAttempts << endl;
    if(newErrors < oldErrors)
    {
      oldErrors = newErrors;
      oldExecutionList = newExecutionList;
      unsuccessfulAttempts = 0;
    }
    else
    {
      unsuccessfulAttempts++;
    }
  }
    
  // reset random seed if another one is using it
  srand((unsigned int)time(NULL));
    
  moduleExecutionList.clear();
  copy(oldExecutionList.begin(), oldExecutionList.end(), back_inserter(moduleExecutionList));
    
  // print execution list
  std::cout << "automatic module execution list" << std::endl;
  std::cout << "-------------------------------" << std::endl;
  for(std::list<std::string>::const_iterator itExec = moduleExecutionList.begin(); 
    itExec != moduleExecutionList.end(); ++itExec
  )
  {
    std::cout << *itExec << std::endl;
  }
  std::cout << "-------------------------------" << std::endl;
  std::cout << std::endl;
    
  // output missing dependencies
  countExecutionListErrors(oldExecutionList, required, provided, true);
    
}//end calculateExecutionListOld

void ModuleManager::internalAddModuleToExecutionList(
  std::string name, 
  std::set<std::string>& availableRepresentations, 
  std::map<std::string,std::string>& providerForRepresentation)
{
  AbstractModuleCreator* am = getModule(name);
  if(am != NULL && am->isEnabled() && am->getModule() != NULL)
  {
    bool addModuleToList = true;
    Module* m = am->getModule();
      
    // add all provided representations of this module to our known set
    const RepresentationMap& provided = m->getProvide(); 
    for(RepresentationMap::const_iterator itProv=provided.begin(); itProv != provided.end(); ++itProv)
    {
      std::string repName = itProv->second->getName();
      if(availableRepresentations.find(repName) == availableRepresentations.end())
      { 
        availableRepresentations.insert(repName);
        providerForRepresentation[repName] = name;
      }
      else
      {
        std::cerr << "ERROR when calculating execution order: " << repName << " provided more than once "
          << "(" << name << " and " << providerForRepresentation[repName] << ")" << std::endl;
          addModuleToList = false;
      }
    } // end for all provided representations
    if(addModuleToList)
    {
      moduleExecutionList.push_back(name);        
    }
  }
} // end internalAddModuleToExecutionList
  
unsigned int ModuleManager::countExecutionListErrors(const std::vector<std::string>& order, 
  std::map<std::string, std::list<std::string> >& required, 
  std::map<std::string, std::list<std::string> >& provided, bool outputError)
{
  unsigned int errors = 0;
    
  std::set<std::string> availableRepresentations;
    
  for(std::vector<std::string>::const_iterator it = order.begin(); it != order.end(); ++it)
  {
    std::string module = *it;
    const std::list<std::string>& req = required[module];
    const std::list<std::string>& prov = provided[module];
      
    for(std::list<std::string>::const_iterator itReq = req.begin(); itReq != req.end(); ++itReq)
    {
      if(availableRepresentations.find(*itReq) == availableRepresentations.end())
      {
        errors++;
        if(outputError)
        {
          std::cerr << "ERROR: no provider for " << *itReq << " in " << module << std::endl;
        }
      }
    }
      
    for(std::list<std::string>::const_iterator itProv = prov.begin(); itProv != prov.end(); ++itProv)
    {
      availableRepresentations.insert(*itProv);
    }
  }
    
  return errors;
}//end countExecutionListErrors
