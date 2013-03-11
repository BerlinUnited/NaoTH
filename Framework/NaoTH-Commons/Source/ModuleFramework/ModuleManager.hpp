/**
* @file ModuleManager.hpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* 
* Implementations of the template stuff of the ModuleManager. It is included in ModuleManager.h
* ACHTUNG: don't include directly (!)
*/

template<class T>
ModuleCreator<T>* ModuleManager::createModule(bool enabled)
{
  return new ModuleCreator<T>(getBlackBoard(), enabled);
}//end createModule


template<class T>
ModuleCreator<T>* ModuleManager::registerModule(std::string name, bool enabled)
{
  // if there is no module creator with the given name
  // add a new entry pointing at NULL
  ModuleCreator<T>* typedModule = NULL;
  ModuleCreatorMap::iterator m = registeredModules.insert(registeredModules.begin(), std::make_pair(name, typedModule));

  // module creator does not exist (it was just added), so create a new one
  if(m->second == NULL)
  {
    typedModule = createModule<T>(enabled);
    moduleExecutionList.push_back(name);
    moduleModuleExecutionList.push_back(typedModule);
    m->second = typedModule;
  }
  else
  {
    // the module creator with the given name already existed, so check if its type 
    // corresponds to the given one
    ModuleCreator<T>* typedModule = dynamic_cast<ModuleCreator<T>*>(m->second);
    if(typedModule == NULL)
    {
      std::cerr << "Module type mismatch: " << name << " is already registered as "
                // HACK: getModuleName doesn't necessary return the type of the module
                << m->second->moduleClassName() << ", but "
                << typeid(T).name() << " requested." << std::endl;
    
      // this is a crucial error => crash
      assert(false);
    }//end if

    typedModule->setEnabled(enabled);
  }

  return typedModule;
}//end registerModule
