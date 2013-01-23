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
#ifndef DEBUG_INFRASTRUCTURE
#define DEBUG_INFRASTRUCTURE
#endif


template<class M> class StaticModuleInterface;

class Module
{
private:
  // pointers to the provided and required representations
  RepresentationMap providedMap;
  RepresentationMap requiredMap;

public:

  Module(){}
  virtual ~Module(){}

  // those have to be overriden
  virtual void execute() = 0;
  virtual std::string getName() const = 0;


  const RepresentationMap& getRequire() const { return requiredMap; }
  const RepresentationMap& getProvide() const { return providedMap; }

public:

  template<class T>
  const T& getRequire(std::string name) const {
    return getRepresentation<T>(requiredMap, name);
  }

  template<class T>
  T& getProvide(const std::string& name) const {
    return getRepresentation<T>(providedMap, name);
  }

  template<class T>
  T& getRepresentation(const RepresentationMap& r_map, const std::string& name) const 
  {
    RepresentationMap::const_iterator iter = r_map.find(name);
    assert(iter != r_map.end());
    DataHolder<T>* rep = dynamic_cast<DataHolder<T>*>(iter->second);
    assert(rep != NULL);
    return **rep;
  }

  template<class M> friend class StaticModuleInterface;
};//end class Module

/** */
std::ostream& operator <<(std::ostream &stream, const Module& module);


/**
* A prototype template for an interface
*/
template<class T> class IF {};



template<class T>
class ST
{
public:
  static T instance;
public:
  static T& getInstance() { return instance; }
};

template<class T>
T ST<T>::instance;


/**
* 
*/
template<class T>
class StaticRegistry
{
protected:
  StaticRegistry(){}
  
private:
  static RegistrationInterfaceRegistry registry;

public:
  static RegistrationInterfaceMap& getProvide() { return ST<IF<T> >::getInstance().registry.getProvide(); }
  static RegistrationInterfaceMap& getRequire() { return ST<IF<T> >::getInstance().registry.getRequire(); }

public:
  template<class R>
  static RegistrationInterface* registerProvide(const std::string& name)
  {
    return registerTypedInterface<R>(registry.getProvide(), name);
  }

  template<class R>
  static RegistrationInterface* registerRequire(const std::string& name)
  {
    return registerTypedInterface<R>(registry.getRequire(), name);
  }

  template<class R>
  static RegistrationInterface* registerTypedInterface(RegistrationInterfaceMap& rr_map, const std::string& name)
  {
    RegistrationInterfaceMap::iterator i = rr_map.find(name);
    if(i == rr_map.end())
      i = rr_map.insert(rr_map.begin(), std::make_pair(name, new TypedRegistrationInterface<R>(name)));

    return i->second;
  }


  template<class R>
  class StaticRequireRegistrator
  {
  public:
    StaticRequireRegistrator(const std::string& name)
    {
      registerRequire<R>(name);
    }

    const R& get(Module* m, const std::string& name) const
	  {
      assert(m != NULL);
		  return m->getRequire<R>(name);
	  }
  };//end StaticRequireRegistrator


  template<class R>
  class StaticProvideRegistrator
  {
  public:
    StaticProvideRegistrator(const std::string& name)
    {
      registerProvide<R>(name);
    }

    R& get(Module* m, const std::string& name) const
	  {
      assert(m != NULL);
		  return m->getProvide<R>(name);
	  }
  };//end StaticProvideRegistrator
};//end class StaticRegistry


template<class T> RegistrationInterfaceRegistry StaticRegistry<T>::registry;


/**
 * @class
 */
class ModuleHolder
{
public:
  void bind(Module* m)
  {
    assert(module == NULL);
    module = m;
  }

  ModuleHolder() : module(NULL){}

protected:
  Module* module;
};


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
#ifdef DEBUG_INFRASTRUCTURE
    std::cout << "Load StaticModuleInterface" << std::endl;
#endif

    registerRequire(StaticRegistry<M>::getRequire());
    registerProvide(StaticRegistry<M>::getProvide());
    
    IF<M>::bind(this);
  }
  
  virtual ~StaticModuleInterface()
  {
    unregister(providedMap);
    unregister(requiredMap);
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
      representation.registerProvide(*this);
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
      representation.registerRequire(*this);
    }//end for
  }//end registerRequire


  void unregister(RepresentationMap& r_map)
  {
    RepresentationMap::iterator iter = r_map.begin();
    for(;iter != r_map.end(); ++iter)
    {
      iter->second->unregisterRequire(*this);
    }
    r_map.clear();
  }//end unregister
};//end class StaticModuleInterface




// internal: don't use directly
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
  class IF<M>:                                          \
    public StaticRegistry<M>,                        \
    public ModuleHolder \
  {                                                     \
    typedef M ModuleType;



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
    STATIC_REGISTRATOR(Provide,R) the##R;               \
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
