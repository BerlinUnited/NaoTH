/**
* @file Module.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Module (base class for modules)
*/

#ifndef _Module_h_
#define _Module_h_

#include "DataHolder.h"
#include "BlackBoardInterface.h"
#include "RegistrationInterface.h"

#include <string.h>
#include <map>
#include <list>

// TODO: remove it (it's not easy)
// ACHTUNG: modules should not be included by anyone else, so that's kind of ok
//          This is ugly, but for now it's the most elegant and simple way i can think of
using namespace naoth;

/** type for a named map of representations */
typedef std::map<std::string, Representation*> RepresentationMap;

/**
* @class Module
* macros for creating a module
* usage:
@code

 #include "RepresentationA.h"
 #include "RepresentationB.h"

  BEGIN_DECLARE_MODULE(ModuleA)
    REQUIRE(RepresentationA)
    PROVIDE(RepresentationB)
  END_DECLARE_MODULE(ModuleA)


  class ModuleA: private ModuleABase
  {

  public:

    // a default constructor is required,
    // it is used to create an instance of the module
    ModuleA()
    {
    }

    // the execute method is called to run the module
    // put your functionality here
    virtual void execute()
    {
      // do something with RepresentationA and RepresentationB:
      // int x = getRepresentationA().x;
      // getRepresentationB().y = x + 1;
    }

  };//end class ModuleA

@endcode
*/

template<class M> class StaticInterface;

class Module
{
// close access to the getBlackBoard for all derived classes
protected:
  // pointers to the provided and required representations
  RepresentationMap providedMap;
  RepresentationMap requiredMap;

public:
  Module()
  {
    #ifdef DEBUG_INFRASTRUCTURE
      std::cout << "Load " << getModuleName() << endl;
    #endif // DEBUG_INFRASTRUCTURE
  }

  virtual ~Module(){}

  /** executes the module */
  virtual void execute() = 0;

  /** */
  virtual std::string getName() const = 0;

  /** */
  const RepresentationMap& getRequire() const
  {
    return requiredMap;
  }

  /** */
  const RepresentationMap& getProvide() const
  {
    return providedMap;
  }

protected:
  /**
  */
  template<class T>
  const T& getRequire(std::string name) const
  {
    RepresentationMap::const_iterator iter = requiredMap.find(name);
    assert(iter != requiredMap.end());
    return **(dynamic_cast<DataHolder<T>* >(iter->second));
  }//end getRequire

  /**
  */
  template<class T>
  T& getProvide(std::string name) const
  {
    RepresentationMap::const_iterator iter = providedMap.find(name);
    assert(iter != providedMap.end());
    return **(dynamic_cast<DataHolder<T>* >(iter->second));
  }//end getProvide

  template<class M> friend class StaticInterface;
};//end class Module


/**
* A prototype template for an interface
*/
template<class T> class IF{};


/**
* 
*/
template<class T>
class StaticRegistry: 
  public RegistrationInterfaceRegistry,
  public Singleton<StaticRegistry<T> >,
  public IF<T>
{
private:
  StaticRegistry(){}
  friend class Singleton<StaticRegistry<T> >;
public:
  static RegistrationInterfaceMap& getProvide() { return getInstance().RegistrationInterfaceRegistry::getProvide(); }
  static RegistrationInterfaceMap& getRequire() { return getInstance().RegistrationInterfaceRegistry::getRequire(); }
};//end class StaticRegistry


/**
 * @class
 */
template<class M>
class StaticInterface
{
public:
  template<class R>
  class StaticRegistrator
  {
  public:
    StaticRegistrator(RegistrationInterfaceMap& static_registry, const std::string& name)
    {
      RegistrationInterface* ri = NULL;
      RegistrationInterfaceMap::iterator i = 
        static_registry.insert(static_registry.begin(), std::make_pair(name, ri));

      if(i->second == NULL)
        i->second = new TypedRegistrationInterface<R>(name);
    }
  };


  template<class R>
  class StaticRequireRegistrator: public StaticRegistrator<R>
  {
  public:
    StaticRequireRegistrator(const std::string& name)
      : StaticRegistrator<R>(getRegistry().getRequire(), name)
    {}

    const R& get(Module* m, const std::string& name) const
		{
      assert(m != NULL);
			return m->getRequire<R>(name);
		}
  };

  template<class R>
  class StaticProvideRegistrator: public StaticRegistrator<R>
  {
  public:
    StaticProvideRegistrator(const std::string& name)
      : StaticRegistrator<R>(getRegistry().getProvide(), name)
    {}

    R& get(Module* m, const std::string& name) const
		{
      assert(m != NULL);
			return m->getProvide<R>(name);
		}
  };

public:
  static StaticRegistry<M>& getRegistry(){ return StaticRegistry<M>::getInstance(); }
  
  void bind(Module* m)
  {
    assert(module == NULL);
    module = m;
  }

  StaticInterface() : module(NULL)
  {
  }

protected:
  Module* module;
};// end class StaticInterface



// static module interface
template<class M>
class StaticModuleInterface: 
  public IF<M>, 
  protected Module,
  virtual protected BlackBoardInterface
{
// close access to the getBlackBoard for all derived classes
private: using BlackBoardInterface::getBlackBoard;

public:
  StaticModuleInterface()
  {
    registerRequire(StaticRegistry<M>::getRequire());
    registerProvide(StaticRegistry<M>::getProvide());
    
    IF<M>::bind(this);
  }
  
  virtual ~StaticModuleInterface()
  {
    unregisterRequire();
    unregisterProvide();
  }

private:
  void registerProvide(const RegistrationInterfaceMap& rr_map)
  {
    RegistrationInterfaceMap::const_iterator iter = rr_map.begin();

    for(;iter != rr_map.end(); ++iter)
    {
      // init the actual dependency to te black board
      Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
      providedMap[iter->first] = &representation;

      // register this module at the representation
      representation.registerProvidingModule(*this);
    }//end for
  }//end registerProvide
  
  void registerRequire(const RegistrationInterfaceMap& rr_map)
  {
    RegistrationInterfaceMap::const_iterator iter = rr_map.begin();

    for(;iter != rr_map.end(); ++iter)
    {
      // init the actual dependency to te black board
      Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
      requiredMap[iter->first] = &representation;

      // register this module at the representation
      representation.registerRequiringModule(*this);
    }//end for
  }//end registerRequire
  
  void unregisterProvide()
  {
    RepresentationMap::iterator iter = providedMap.begin();
    for(;iter != providedMap.end(); ++iter)
    {
      iter->second->unregisterProvidingModule(*this);
    }
    providedMap.clear();
  }//end unregisterProvide
 

  void unregisterRequire()
  {
    RepresentationMap::iterator iter = requiredMap.begin();
    for(;iter != requiredMap.end(); ++iter)
    {
      iter->second->unregisterProvidingModule(*this);
    }
    requiredMap.clear();
  }//end unregisterRequire
};//end class StaticModuleInterface



#define STATIC_REGISTRATOR(N,R)                         \
  class Register##R: public Static##N##Registrator<R>   \
  {                                                     \
  public:                                               \
    Register##R(): Static##N##Registrator<R>(#R){}      \
  }

//
#define BEGIN_DECLARE_MODULE(M)                         \
  class M;                                              \
  template<>                                            \
  class IF<M>: public StaticInterface<M>                \
  {

//
#define REQUIRE(R)                                      \
  private:                                              \
    STATIC_REGISTRATOR(Require,R) the##R;               \
  protected:                                            \
    inline const R& get##R() const                      \
    {                                                   \
      static const R& data = the##R.get(module, #R);    \
      return data;                                      \
    }

//
#define PROVIDE(R)                                      \
  private:                                              \
  STATIC_REGISTRATOR(Provide,R) the##R;                 \
  protected:                                            \
    inline R& get##R() const                            \
    {                                                   \
      static R& data = the##R.get(module, #R);          \
      return data;                                      \
    }


//
#define END_DECLARE_MODULE(M)                           \
  };                                                    \
  class M##Base: public StaticModuleInterface<M>        \
  {                                                     \
  public: std::string getName() const { return #M; }    \
  };

#endif // _Module_h_
