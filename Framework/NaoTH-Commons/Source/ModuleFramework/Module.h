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
using namespace naoth;


/** type for a named map of representations */
typedef std::map<std::string, RegistrationInterface*> RepresentationMap;


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
class Module: virtual protected BlackBoardInterface
{
private:
  std::string name;

// close access to the getBlackBoard for all derived classes
private:
    using BlackBoardInterface::getBlackBoard;

protected:

  // pointers to the provided and required representations
  std::list<Representation*> provided;
  std::list<Representation*> required;

  std::map<std::string, Representation*> providedMap;
  std::map<std::string, Representation*> requiredMap;

  Module(std::string name): name(name)
  {
    // std::cout << "Load " << getModuleName() << endl;
  }

  // TODO: remove, make it tool methods
  void registerProviding(const RepresentationMap& list)
  {
    RepresentationMap::const_iterator iter = list.begin();
    for(;iter != list.end(); iter++)
    {
      // init the actual dependency to te black board
      Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
      provided.push_back(&representation);
      providedMap[iter->first] = &representation;

      representation.registerProvidingModule(*this);
    }
  }
  
  void registerRequiring(const RepresentationMap& list)
  {
    RepresentationMap::const_iterator iter = list.begin();
    for(;iter != list.end(); iter++)
    {
      // init the actual dependency to te black board
      Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
      required.push_back(&representation);
      requiredMap[iter->first] = &representation;

      representation.registerRequiringModule(*this);
    }
  }
  
  void unregisterProviding(const RepresentationMap& list)
  {
    RepresentationMap::const_iterator iter = list.begin();
    for(;iter != list.end(); iter++)
    {
      // init the actual dependency to te black board
      Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
      //provided.remove(&representation);
      representation.unregisterProvidingModule(*this);
    }
  } 
 
  void unregisterRequiring(const RepresentationMap& list)
  {
    RepresentationMap::const_iterator iter = list.begin();
    for(;iter != list.end(); iter++)
    {
      // init the actual dependency to te black board
      Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
      //required.remove(&representation);
      representation.unregisterRequiringModule(*this);
    }
  }

public:
  const std::list<Representation*>& getRequiredRepresentations() const
  {
    return required;
  }

  const std::list<Representation*>& getProvidedRepresentations() const
  {
    return provided;
  }

  /** executes the module */
  virtual void execute() = 0;


  virtual ~Module() {}
  const std::string& getModuleName() const { return (*this).name; }
  
  // what is it used for?
  Module* operator->(){ return this; }
  //friend ostream& operator<<(ostream& stream, Module& module);
};//end class module


/**
 * @class
 */
template<class T>
class StaticRegistry
{
private:
  RepresentationMap provide_registry_map;
  RepresentationMap require_registry_map;

protected:
  static StaticRegistry<T>* instance;
  static RepresentationMap& provide_registry(){ return instance->provide_registry_map; }
  static RepresentationMap& require_registry(){ return instance->require_registry_map; }


  template<class TYPE_WHAT>
  class StaticRegistrator//: public RegistrationInterface
  {
  protected:
    // pointer to the actual data on the blackboard
    TYPE_WHAT* data;
    // the name of the representation
    std::string name;

  public:
    StaticRegistrator() : data(NULL){}

    StaticRegistrator(RepresentationMap& static_registry, const std::string& name)
      : 
        data(NULL),
        name(name)
    {
      // TODO: check the type
      if(static_registry.find(name) == static_registry.end())
      {
        static_registry[name] = new TypedRegistrationInterface<TYPE_WHAT>(name);
      }
    }

    DataHolder<TYPE_WHAT>& get(BlackBoard& blackBoard)
    {
      return blackBoard.template getRepresentation<DataHolder<TYPE_WHAT> >(name);
    }

    TYPE_WHAT& get(const std::map<std::string, Representation*>& list, std::string name) const
    {
      std::map<std::string, Representation*>::const_iterator iter = list.find(name);
      assert(iter != list.end());
      return **(dynamic_cast<DataHolder<TYPE_WHAT>* >(iter->second));
    }

    
  };


  template<class TYPE_WHAT>
  class StaticRequireRegistrator: public StaticRegistrator<TYPE_WHAT>
  {
    // seems to be necessary for gcc to resolve the member data
    typedef StaticRegistrator<TYPE_WHAT> ParentT;

  public:
    StaticRequireRegistrator(const std::string& name)
      : 
      StaticRegistrator<TYPE_WHAT>(require_registry(), name)
    {
    }

    inline const TYPE_WHAT& getData() const
    {
      assert(ParentT::data != NULL);
      return *ParentT::data;
    }
  };

  template<class TYPE_WHAT>
  class StaticProvideRegistrator: public StaticRegistrator<TYPE_WHAT>
  {
    // seems to be necessary for gcc to resolve the member data
    typedef StaticRegistrator<TYPE_WHAT> ParentT;

  public:
    StaticProvideRegistrator(const std::string& name)
      : 
      StaticRegistrator<TYPE_WHAT>( provide_registry(), name)
    {
    }

    inline TYPE_WHAT& getData() const
    {
      assert(ParentT::data != NULL);
      return *ParentT::data;
    }
  };
}; // end class StaticRegistry

template<class T> 
StaticRegistry<T>* StaticRegistry<T>::instance = new StaticRegistry<T>();


class U : public StaticRegistry<U>
{
public:

  class R 
  {
  } theR;

};


/*
template<class T>
RepresentationMap* StaticRegistry<T>::static_provide_registry = new RepresentationMap();
template<class T>
RepresentationMap* StaticRegistry<T>::static_require_registry = new RepresentationMap();
*/

// helper, should not be used outside
// static invoker (registers the static dependency to RepresentationB)
#define _CREATE_REGISTRATOR(R, T)                       \
  class R##T : public T<R>                              \
  {                                                     \
  public:                                               \
    R##T() : T<R>(#R){}                                 \
    template<class S> T<R> S::* getMember(){ return &S::the##R; } \
  } the##R;


//
#define BEGIN_DECLARE_MODULE(M)                         \
  class M##Base:                             \
    protected Module, \
    protected StaticRegistry<M##Base>        \
  { 

//
#define REQUIRE(R)                                      \
  private:                                              \
    _CREATE_REGISTRATOR(R, StaticRequireRegistrator);   \
  protected:                                            \
    inline const R& get##R() const                      \
    {                                                   \
      static R& data = the##R.get(requiredMap,#R);          \
      return data; \
    }

//
#define PROVIDE(R)                                      \
  private:                                              \
    _CREATE_REGISTRATOR(R, StaticProvideRegistrator);   \
  protected:                                            \
    inline R& get##R() const                            \
    {                                                   \
      static R& data = the##R.get(providedMap,#R);   \
      return data;                           \
    }


//
#define END_DECLARE_MODULE(M)                           \
  public:                                               \
    M##Base(): Module(#M){                              \
      registerRequiring(require_registry());      \
      registerProviding(provide_registry());      \
    }                                                   \
    virtual ~M##Base(){                                 \
      unregisterRequiring(require_registry());    \
      unregisterProviding(provide_registry());    \
    }                                                   \
    static std::string const className() { return #M; } \
  };

#endif // _Module_h_
