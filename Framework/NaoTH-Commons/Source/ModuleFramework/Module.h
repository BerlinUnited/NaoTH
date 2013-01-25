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
* A prototype template for a static interface of a module
*/
template<class T> class IF { typedef Module ModuleType; };


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

class Module: protected virtual BlackBoardInterface
{
// close access to the getBlackBoard for all derived classes
private: using BlackBoardInterface::getBlackBoard;

private:
  // pointers to the provided and required representations
  RepresentationMap providedMap;
  RepresentationMap requiredMap;

public:

  Module(){}
  virtual ~Module() {
    unregister(providedMap);
    unregister(requiredMap);
  }

  // those have to be overriden
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

protected:
  void registerProvide(const RegistrationInterfaceMap& rr_map);
  void registerRequire(const RegistrationInterfaceMap& rr_map);
  void unregister(RepresentationMap& r_map);

  template<class T> friend class IF;
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
* 
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
    if(!init) { IF<T>(); init = true; } 
    return registryRequire.registry; 
  }
  static const RegistrationInterfaceMap& getProvide() { 
    if(!init) { IF<T>(); init = true; } 
    return registryProvide.registry; 
  }
};//end class StaticRegistry


template<class T> bool StaticRegistry<T>::init = false;
template<class T> RegistrationInterfaceRegistry StaticRegistry<T>::registryRequire;
template<class T> RegistrationInterfaceRegistry StaticRegistry<T>::registryProvide;


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

// specialize the interface IF<M> for the module M
#define BEGIN_DECLARE_MODULE(M)                               \
  class M;                                                    \
  template<> class IF<M>: public StaticRegistry<M>            \
  {                                                           \
  public:                                                     \
    typedef M ModuleType;                                     \
    virtual const Module* getModule() const {                 \
      assert(false); return NULL; \
    }



#define REQUIRE(R)                                            \
  private: STATIC_REGISTRATOR(Require,R) the##R;              \
  protected: inline const R& get##R() const {                 \
      static const R& data = getModule()->getRequire<R>(#R);  \
      return data;                                            \
    }


#define PROVIDE(R)                                            \
  private: STATIC_REGISTRATOR(Provide,R) the##R;              \
  protected: inline R& get##R() const {                       \
      static R& data = getModule()->getProvide<R>(#R);        \
      return data;                                            \
    }


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
