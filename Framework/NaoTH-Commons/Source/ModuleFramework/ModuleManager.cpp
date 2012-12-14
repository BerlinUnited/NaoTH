

#include "ModuleManager.h"

#include <ctime>

ModuleManager::~ModuleManager()
{
  std::map<std::string, AbstractModuleCreator* >::iterator iter;
  for(iter = moduleExecutionMap.begin(); iter != moduleExecutionMap.end(); iter++)
  {
    delete (iter->second);
  }//end for
}//end destructor


void ModuleManager::setModuleEnabled(std::string moduleName, bool value, bool recalculateExecutionList)
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


AbstractModuleCreator* ModuleManager::getModule(const std::string& name)
{
  std::map<std::string, AbstractModuleCreator* >::iterator iter = moduleExecutionMap.find(name);
  if(iter != moduleExecutionMap.end())
  {
    return iter->second;
  }

  return NULL;
}//end getModule


const AbstractModuleCreator* ModuleManager::getModule(const std::string& name) const
{
  std::map<std::string, AbstractModuleCreator* >::const_iterator iter = moduleExecutionMap.find(name);
  if(iter != moduleExecutionMap.end())
  {
    return iter->second;
  }

  return NULL;
}//end getModule



void ModuleManager::calculateExecutionList()
{
    
  bool changed = true;
  std::list<std::string>::iterator start=moduleExecutionList.begin();
  const int maxAttempts = moduleExecutionList.size()*10;
  int iterations = 0;

  while(changed && iterations < maxAttempts)
  {
    changed = false;
    iterations++;


    for(std::list<std::string>::iterator it1=start; it1 != moduleExecutionList.end(); it1++)
    {
      start = it1;
      if(!moduleExecutionMap[*it1]->isEnabled()) continue;
      Module* m1 = moduleExecutionMap[*it1]->getModule();

      for(std::list<std::string>::iterator it2=it1; it2 != moduleExecutionList.end(); it2++)
      {
        if(!moduleExecutionMap[*it2]->isEnabled()) continue;
        Module* m2 = moduleExecutionMap[*it2]->getModule();

        for(std::list<Representation*>::const_iterator itReq = m1->getRequiredRepresentations().begin();
          itReq != m1->getRequiredRepresentations().end(); itReq++)
        {
          std::string repName = (*itReq)->getName();
          for(std::list<Representation*>::const_iterator r = m2->getProvidedRepresentations().begin();
            r != m2->getProvidedRepresentations().end(); ++r)
          {
            if((*r)->getName() == repName)
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

  if(iterations >= maxAttempts)
  {
    std::cerr << "WARNING: maximal number of iterations reached." << std::endl;
  }//end if


  // print execution list
  std::cout << "automatic module execution list" << std::endl;
  std::cout << "-------------------------------" << std::endl;
  for(std::list<std::string>::const_iterator itExec = moduleExecutionList.begin(); 
    itExec != moduleExecutionList.end(); itExec++
  )
  {
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
  for(std::map<std::string, AbstractModuleCreator* >::const_iterator it=moduleExecutionMap.begin(); 
    it != moduleExecutionMap.end(); it++)
  {
    // only include enabled modules
    if(it->second->isEnabled())
    {
      Module* m = it->second->getModule();
        
      for(std::list<Representation*>::const_iterator itProv = m->getProvidedRepresentations().begin();
        itProv != m->getProvidedRepresentations().end(); itProv++)
      {
        std::string repName = (*itProv)->getName();
          
        if(providerForRepresentation.find(repName) == providerForRepresentation.end())
        {
          providerForRepresentation[repName] = it->first;
          provided[it->first].push_back(repName);
        }
        else
        {
          std::cerr << "ERROR: " << repName << " provided by both " << providerForRepresentation[repName]
            << " and " << it->first;
        }
      }//end for
        
      for(std::list<Representation*>::const_iterator itReq = m->getRequiredRepresentations().begin();
        itReq != m->getRequiredRepresentations().end(); itReq++)
      {
        std::string repName = (*itReq)->getName();
        required[it->first].push_back(repName);
      }//end for
        
    } // if module enabled
  }//end for
    
  // solve this constraint problem
  std::vector<std::string> oldExecutionList(moduleExecutionList.begin(), moduleExecutionList.end());
  unsigned int oldErrors = countExecutionListErrors(oldExecutionList, required, provided, false);
  unsigned int unsuccessfulAttempts = 0;
    
  const unsigned int maxAttempts = oldExecutionList.size()*10;
    
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
      left = rand() % newExecutionList.size();
      right = rand() % newExecutionList.size();
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
    itExec != moduleExecutionList.end(); itExec++
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
    const std::list<Representation*> provided = m->getProvidedRepresentations(); 
    for(std::list<Representation*>::const_iterator itProv=provided.begin(); itProv != provided.end(); itProv++)
    {
      std::string repName = (*itProv)->getName();
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
    
  for(std::vector<std::string>::const_iterator it = order.begin(); it != order.end(); it++)
  {
    std::string module = *it;
    const std::list<std::string>& req = required[module];
    const std::list<std::string>& prov = provided[module];
      
    for(std::list<std::string>::const_iterator itReq = req.begin(); itReq != req.end(); itReq++)
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
      
    for(std::list<std::string>::const_iterator itProv = prov.begin(); itProv != prov.end(); itProv++)
    {
      availableRepresentations.insert(*itProv);
    }
  }
    
  return errors;
}//end countExecutionListErrors
