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

#include <string.h>
#include <map>
#include <list>

// TODO: remove it (it's not easy)
using namespace naoth;


/**
 * 
 */
class RegistrationInterface
{
public:
  virtual Representation& registerAtBlackBoard(BlackBoard& blackBoard) = 0;
};


/** type for a named map of representations */
typedef std::map<std::string, RegistrationInterface*> RepresentationMap;


/**
 *
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
      provided.remove(&representation);
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
      required.remove(&representation);
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
};


/**
 *
 */
template<class T>
class StaticRegistry
{
protected:
  static RepresentationMap* static_provide_registry;
  static RepresentationMap* static_require_registry;
  

  template<class TYPE_WHAT>
  class StaticRegistrator: public RegistrationInterface
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
        static_registry[name] = this;
      }
    }

    virtual Representation& registerAtBlackBoard(BlackBoard& blackBoard)
    {
      DataHolder<TYPE_WHAT>& rep = blackBoard.template getRepresentation<DataHolder<TYPE_WHAT> >(name);
      data = &(*rep);
      return rep;
    }
  };


  template<class TYPE_WHAT>
  class StaticRequireRegistrator: public StaticRegistrator<TYPE_WHAT>
  {
  public:
    StaticRequireRegistrator(const std::string& name)
      : 
      StaticRegistrator<TYPE_WHAT>(*static_require_registry, name)
    {
    }

    inline const TYPE_WHAT& getData() const
    {
      assert(data != NULL);
      return *data;
    }
  };

  template<class TYPE_WHAT>
  class StaticProvideRegistrator: public StaticRegistrator<TYPE_WHAT>
  {
  public:
    StaticProvideRegistrator(const std::string& name)
      : 
      StaticRegistrator<TYPE_WHAT>(*static_provide_registry, name)
    {
    }

    inline TYPE_WHAT& getData() const
    {
      assert(data != NULL);
      return *data;
    }
  };
};


template<class T>
RepresentationMap* StaticRegistry<T>::static_provide_registry = new RepresentationMap();
template<class T>
RepresentationMap* StaticRegistry<T>::static_require_registry = new RepresentationMap();


/***************************************************************
 macros for creating a module
 usage:

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

****************************************************************/

// helper, should not be used outside
// static invoker (registers the static dependency to RepresentationB)
#define _CREATE_REGISTRATOR(R, T) \
  class R##T : public T<R> \
  { \
  public: \
    R##T() : T<R>(#R){} \
  } the##R;


//
#define BEGIN_DECLARE_MODULE(M) \
  class M##Base: \
    protected Module, \
private StaticRegistry<M##Base> \
  { 


// 
#define REQUIRE(R) \
  private: \
    _CREATE_REGISTRATOR(R, StaticRequireRegistrator); \
  protected: \
    inline const R& get##R() const \
    { \
      return the##R.getData(); \
    }

//
#define PROVIDE(R) \
  private: \
    _CREATE_REGISTRATOR(R, StaticProvideRegistrator); \
  protected: \
    inline R& get##R() const \
    { \
      return the##R.getData(); \
    }

//
#define END_DECLARE_MODULE(M) \
  public: \
    M##Base(): Module(#M){ \
      registerRequiring(*static_require_registry); \
      registerProviding(*static_provide_registry); \
    } \
    virtual ~M##Base(){ \
      unregisterRequiring(*static_require_registry); \
      unregisterProviding(*static_provide_registry); \
    } \
    static std::string const className() { return #M; } \
  };

#endif // _Module_h_
