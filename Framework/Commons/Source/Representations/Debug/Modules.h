/* 
 * File:   Modules.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef _Modules_H_
#define _Modules_H_

#include <map>
#include <cstring>
#include <string>


#include "ModuleFramework/BlackBoard.h"
#include "ModuleFramework/ModuleCreator.h"

#include "Tools/DataStructures/Serializer.h"
#include "Tools/DataStructures/Printable.h"

class Modules: virtual public BlackBoardInterface, public Printable
{
public:
  typedef std::map<std::string, AbstractModuleCreator* > ModuleCreatorMap;

private:
  ModuleCreatorMap modules;

public:
  const ModuleCreatorMap& getModules() const {
    return modules;
  }

  ~Modules()
  {
  }

  AbstractModuleCreator* getModule(const std::string& name)
  {
    ModuleCreatorMap::const_iterator iter = modules.find(name);
    if(iter != modules.end()) {
      return iter->second;
    }

    return NULL;
  }

  const AbstractModuleCreator* getModule(const std::string& name) const
  {
    ModuleCreatorMap::const_iterator iter = modules.find(name);
    if(iter != modules.end()) {
      return iter->second;
    }

    return NULL;
  }
  
  template<class T>
  ModuleCreator<T>* registerModule(const std::string& name, bool enabled)
  {
    // if there is no module creator with the given name
    // add a new entry pointing at NULL
    ModuleCreator<T>* typedModule = NULL;
    ModuleCreatorMap::iterator m = modules.insert(modules.begin(), std::make_pair(name, typedModule));

    // module creator does not exist (it was just added), so create a new one
    if(m->second == NULL)
    {
      //typedModule = new ModuleCreator<T>(getBlackBoard(), enabled);
      typedModule = &(getBlackBoard().getRepresentation<DataHolder<ModuleCreator<T> > >(name));
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
    }

    typedModule->setEnabled(enabled);

    return typedModule;
  }//end registerModule

  virtual void print(std::ostream& stream) const
  {
    ModuleCreatorMap::const_iterator iter;
    for (iter = modules.begin(); iter != modules.end(); ++iter) {
      stream << "[" << iter->second->isEnabled() << "] " << iter->first << std::endl;
      // TODO: print representations
    }
  }
};

namespace naoth
{
template<>
class Serializer<Modules>
{
  public:
  static void serialize(const Modules& object, std::ostream& stream);

  // we don't need that
  static void deserialize(std::istream& stream, Modules& object);
};
}

#endif  /* _Modules_H_ */

