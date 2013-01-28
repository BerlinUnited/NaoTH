/**
* @file Module.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*
* 
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
* A prototype for a static interface of a module
*/
template<class T> class IF;


/**
* @class Module
*/
class Module: protected virtual BlackBoardInterface
{
// close access to the getBlackBoard for all derived classes
private: using BlackBoardInterface::getBlackBoard;

public:
  Module(){}
  virtual ~Module() {
    unregister(providedMap);
    unregister(requiredMap);
  }

  virtual void execute() = 0;
  virtual std::string getName() const = 0;

  const RepresentationMap& getRequire() const { return requiredMap; }
  const RepresentationMap& getProvide() const { return providedMap; }

protected:

  template<class T>
  const T& getRequire(std::string name) const {
    return getRepresentation<T>(requiredMap, name);
  }

  template<class T>
  T& getProvide(const std::string& name) const {
    return getRepresentation<T>(providedMap, name);
  }

  template<class T>
  T& getRepresentation(const RepresentationMap& r_map, const std::string& name) const;

  // the interface IF<T> uses the methods getProvide and getRequire
  template<class T> friend class IF;

protected:
  void registerProvide(const RegistrationInterfaceMap& rr_map);
  void registerRequire(const RegistrationInterfaceMap& rr_map);
  void unregister(RepresentationMap& r_map);

private:
  // pointers to the provided and required representations
  // they are filled at the construction time of the module by the methods
  // registerProvide and registerRequire
  RepresentationMap providedMap;
  RepresentationMap requiredMap;

};//end class Module


template<class T>
T& Module::getRepresentation(const RepresentationMap& r_map, const std::string& name) const 
{
  RepresentationMap::const_iterator iter = r_map.find(name);
  assert(iter != r_map.end());
  DataHolder<T>* rep = dynamic_cast<DataHolder<T>*>(iter->second);
  assert(rep != NULL);
  return **rep;
}

/** */
std::ostream& operator <<(std::ostream &stream, const Module& module);


/**
* Holds static maps with registrators for required/provided representations
* The fields registryRequire and registryProvide are filled by IF<T> during it's
* construction time. 
*/
template<class T>
class StaticRegistry
{
private:
  static bool init;
protected:
  static RegistrationInterfaceRegistry registryRequire;
  static RegistrationInterfaceRegistry registryProvide;
public:
  static const RegistrationInterfaceMap& getRequire() { 
    // call the constructor of IF<T> once to initialize registryRequire and registryProvide
    // this construction compiles faster than a static variable of type IF<T>
    if(!init) { IF<T>(); init = true; } 
    return registryRequire.registry; 
  }
  static const RegistrationInterfaceMap& getProvide() { 
    // @see getRequire()
    if(!init) { IF<T>(); init = true; } 
    return registryProvide.registry; 
  }
};

template<class T> bool StaticRegistry<T>::init = false;
template<class T> RegistrationInterfaceRegistry StaticRegistry<T>::registryRequire;
template<class T> RegistrationInterfaceRegistry StaticRegistry<T>::registryProvide;


/**
* A default definition of a static interface of a module
*/
template<class T> class IF : public StaticRegistry<T> {};


/**
* NOTE: an object of a 'registrator' class registers an interface for a 
* representation when it is created.
* The classes STATIC_REQUIRE_REGISTRATOR and STATIC_PROVIDE_REGISTRATOR
* may be implemented much more elegant utilizing templates, 
* but the macros make it possible to use the actual name of the 
* representation class, i.e., #R.
*/

// internal: don't use directly
#define STATIC_REGISTRATOR(N,R)                               \
  class Register##R {                                         \
  public:                                                     \
    Register##R() {                                           \
      registry##N.registerInterface<R>(#R);                   \
    }                                                         \
  }

/** 
* specialize the interface IF<M> for the module M
*/
#define BEGIN_DECLARE_MODULE(M)                               \
  class M;                                                    \
  template<> class IF<M>: public StaticRegistry<M>            \
  {                                                           \
  public:                                                     \
    typedef M ModuleType;                                     \
    virtual const Module* getModule() const {                 \
      assert(false); return NULL; \
    }

/**
* - autoregistration at construction time
* - getter method for internal use, eg getImage()
*/
#define REQUIRE(R)                                            \
  private: STATIC_REGISTRATOR(Require,R) the##R;              \
  protected: inline const R& get##R() const {                 \
      static const R& data = getModule()->getRequire<R>(#R);  \
      return data;                                            \
    }

/**
* same as @see REQUIRE but not const
*/
#define PROVIDE(R)                                            \
  private: STATIC_REGISTRATOR(Provide,R) the##R;              \
  protected: inline R& get##R() const {                       \
      static R& data = getModule()->getProvide<R>(#R);        \
      return data;                                            \
    }

/**
* closse the class IF<T>
* create a base class for the module, eg SelfLocatorBase, 
* which combines the static (IF<M>) and dynamic (Module) parts
*/
#define END_DECLARE_MODULE(M)                                 \
  };                                                          \
  class M##Base: public IF<M>, public Module                  \
  {                                                           \
  public: M##Base() {                                         \
    registerRequire(IF<M>::getRequire());                     \
    registerProvide(IF<M>::getProvide());                     \
  }                                                           \
  public:                                                     \
    virtual const Module* getModule() const { return this; }  \
    std::string getName() const { return #M; }                \
  };

#endif // _Module_h_
