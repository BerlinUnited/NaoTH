/**
* @file Module.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Module (base class for modules)
*/

#ifndef _Module_h_
#define _Module_h_

#include "BlackBoardInterface.h"
#include "RegistrationInterface.h"
//#include "DataHolder.h"

#include <string.h>
#include <map>
#include <list>

// TODO: remove it (it's not easy)
using namespace naoth;

//class Representation;


/** type for a named map of representations */
typedef std::map<std::string, RegistrationInterface*> RepresentationMap;


/**
 *
 */
class Module: virtual protected BlackBoardInterface
{
private:
  std::string moduleName;

// close access to the getBlackBoard for all derived classes
private:
    using BlackBoardInterface::getBlackBoard;

protected:

  // pointers to the provided and required representations
  std::list<Representation*> provided;
  std::list<Representation*> required;

  Module(std::string name): moduleName(name)
  {
    // std::cout << "Load " << getModuleName() << endl;
  }

  Module(): moduleName("invalid module")
  {
    // should never be here
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

  const std::list<Representation*>& getRequiredRepresentations()
  {
    return required;
  }

  const std::list<Representation*>& getProvidedRepresentations()
  {
    return provided;
  }

  /** executes the module */
  virtual void execute() = 0;

  virtual ~Module() {}
  const std::string& getModuleName() const { return (*this).moduleName; }
  
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
  class StaticRequiringRegistrator
  {
  private:
    TypedRegistrationInterface<TYPE_WHAT>* data;

  public:
    StaticRequiringRegistrator() : data(NULL){}

    StaticRequiringRegistrator(const std::string& name)
      : data(NULL)
    {
      // TODO: check the type
      if(static_require_registry->find(name) == static_require_registry->end())
      {
        (*static_require_registry)[name] = new TypedRegistrationInterface<TYPE_WHAT>(name);
      }
      // HACK
      data = (TypedRegistrationInterface<TYPE_WHAT>*)(*static_require_registry)[name];
    }

    const TYPE_WHAT& get(const BlackBoard& blackBoard) const
    {
      return *(data->get(blackBoard));
    }

    inline const TYPE_WHAT& getData() const
    {
      assert(data->data != NULL);
      return *(data->data);
    }
  };


  template<class TYPE_WHAT>
  class StaticProvidingRegistrator
  {
  private:
    TypedRegistrationInterface<TYPE_WHAT>* data;

  public:
    StaticProvidingRegistrator(): data(NULL) {}

    StaticProvidingRegistrator(const std::string& name)
      : data(NULL)
    {
      // TODO: check the type
      if(static_provide_registry->find(name) == static_provide_registry->end())
      {
        (*static_provide_registry)[name] = new TypedRegistrationInterface<TYPE_WHAT>(name);
      }
      // HACK
      data = (TypedRegistrationInterface<TYPE_WHAT>*)(*static_provide_registry)[name];
    }

    inline TYPE_WHAT& get(BlackBoard& blackBoard) const
    {
      return *(data->get(blackBoard));
    }

    inline TYPE_WHAT& getData() const
    {
      assert(data->data != NULL);
      return *(data->data);
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


#define BEGIN_DECLARE_MODULE(moduleName) \
  class moduleName##Base: \
    protected Module, private StaticRegistry<moduleName##Base> \
  { 

// static invoker (registers the static dependency to RepresentationB)
  
#define REQUIRE(representationName) \
  private: \
  class representationName##StaticRequiringRegistrator : public StaticRequiringRegistrator<representationName>\
  { \
  public: representationName##StaticRequiringRegistrator() : StaticRequiringRegistrator<representationName>(#representationName){} \
  } the##representationName; \
  protected: \
  inline const representationName& get##representationName() const \
  { \
    return the##representationName.getData(); \
  }
  /*
    static const DataHolder<representationName>& representation = getBlackBoard().getConstRepresentation<DataHolder<representationName> >(typeid(representationName).name()); \
    return *representation; \
  }
*/

#define PROVIDE(representationName) \
  private: \
  class representationName##StaticRequiringRegistrator : public StaticProvidingRegistrator<representationName>\
  { \
  public: representationName##StaticRequiringRegistrator() : StaticProvidingRegistrator<representationName>(#representationName){} \
  } the##representationName; \
  protected: \
  inline representationName& get##representationName() const \
  { \
    return the##representationName.getData(); \
  }
/*
  inline representationName& get##representationName() const \
  { \
    static representationName& representation = the##representationName.get(getBlackBoard()); \
    return representation; \
  }
  */
/*
    static DataHolder<representationName>& representation = getBlackBoard().getRepresentation<DataHolder<representationName> >(typeid(representationName).name()); \
    return *representation; \
  }
  */


#define END_DECLARE_MODULE(moduleName) \
  public: \
    moduleName##Base(): Module(#moduleName){ \
      registerRequiring(*static_require_registry); \
      registerProviding(*static_provide_registry); \
    } \
    virtual ~moduleName##Base(){ \
      unregisterRequiring(*static_require_registry); \
      unregisterProviding(*static_provide_registry); \
    } \
    static std::string const className() { return #moduleName; } \
  };

#endif //_Module_h_
