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
* A prototype for the accessor class alowing access to a partcular representation n the blackboard
*/
template<class R, class RR> class Accessor;
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
    unregisterRequire();
    unregisterProvide();
  }

  virtual void execute() = 0;
  virtual std::string getName() const = 0;
  virtual std::string getModulePath() const = 0; // experimental
  virtual std::string getDescription() const = 0; // experimental

  const RepresentationMap& getRequire() const { return requiredMap; }
  const RepresentationMap& getProvide() const { return providedMap; }

protected:
  // the interface Accessor<R,RR> uses the methods getBlackBoard()
  template<class R, class RR> friend class Accessor;

protected:
  void registerProvide(Representation& representation) {
    providedMap[representation.getName()] = &representation;
    representation.registerProvide(*this);
  }
  void registerRequire(Representation& representation) {
    requiredMap[representation.getName()] = &representation;
    representation.registerRequire(*this);
  }

  void unregisterRequire();
  void unregisterProvide();

private:
  // pointers to the provided and required representations
  // they are filled at the construction time of the module by the methods
  // registerProvide and registerRequire
  RepresentationMap providedMap;
  RepresentationMap requiredMap;

};//end class Module


/** */
std::ostream& operator <<(std::ostream &stream, const Module& module);

std::string get_sub_core_module_path(std::string fullpath);

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
    return registryRequire.registry(); 
  }
  static const RegistrationInterfaceMap& getProvide() { 
    // @see getRequire()
    if(!init) { IF<T>(); init = true; } 
    return registryProvide.registry(); 
  }
  static std::string description;
};

template<class T> bool StaticRegistry<T>::init = false;
template<class T> RegistrationInterfaceRegistry StaticRegistry<T>::registryRequire;
template<class T> RegistrationInterfaceRegistry StaticRegistry<T>::registryProvide;
template<class T> std::string StaticRegistry<T>::description = "empty";


template<class R, class ReturnType> class Accessor 
{
public:
  Accessor(Module* module, RegistrationInterfaceRegistry& registry, const std::string& name) 
    : 
    reg(nullptr),
    data(nullptr)
  {
    reg = registry.registerInterface<R>(name);

    if(module != nullptr) 
    {
      Representation& representation = reg->registerAtBlackBoard(module->getBlackBoard());
      
      // register the representation
      if(std::is_const<ReturnType>::value) {
        module->registerRequire(representation);
      } else {
        module->registerProvide(representation);
      }
      
      // store for fast access
      DataHolder<R>* rep = dynamic_cast<DataHolder<R>*>(&representation);
      data = &(**rep);
      assert(data != nullptr);
    }
  }

  inline ReturnType& operator() () const { return *data; }

private: 
  RegistrationInterface *reg;

  R* data;
};

template<class T> using Require = Accessor<T, const T>;
template<class T> using Provide = Accessor<T, T>;


/**
* A default definition of a static interface of a module
*/
template<class T> class IF : public StaticRegistry<T> {};

/** 
* specialize the interface IF<M> for the module M
*/
#define BEGIN_DECLARE_MODULE(M)                               \
  class M;                                                    \
  template<> class IF<M>: public StaticRegistry<M>            \
  {                                                           \
  private: Module* module;                                    \
  public:                                                     \
    IF() : module(nullptr) {}                                 \
    IF(Module* module) : module(module) {}                    \
    const Module& getModule() const { return *module; }       \
    static std::string getName() { return #M; }               \
    static std::string getModulePath() {                      \
      return get_sub_core_module_path(__FILE__);              \
    }
    

/**
* - autoregistration at construction time
* - getter method for internal use, eg getImage()
*/
#define REQUIRE(R) \
  protected: Require<R> get##R = Require<R>(module, registryRequire, #R);

/**
* same as @see REQUIRE but not const
*/
#define PROVIDE(R) \
  protected: Provide<R> get##R = Provide<R>(module, registryProvide, #R);

/**
* closse the class IF<T>
* create a base class for the module, eg SelfLocatorBase, 
* which combines the static (IF<M>) and dynamic (Module) parts
*/
#define END_DECLARE_MODULE(M)                                 \
  };                                                          \
  class M##Base: public Module, public IF<M>                  \
  {                                                           \
  public: M##Base() : IF<M>(this) {                           \
  }                                                           \
  public:                                                     \
    virtual std::string getName() const {return #M; }         \
    virtual std::string getModulePath() const { return IF<M>::getModulePath(); } \
    virtual std::string getDescription() const { return IF<M>::description; } \
  };

#endif // _Module_h_
