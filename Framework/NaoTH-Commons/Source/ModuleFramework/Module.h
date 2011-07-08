/**
* @file Module.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class Module (base class for modules)
*/

#ifndef __Module_h_
#define __Module_h_

#include "BlackBoardInterface.h"

#include "DataHolder.h"

#include <string.h>
#include <map>
#include <list>


using namespace std;

class Representation;

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
template<class T>
class TypedRegistrationInterface: public RegistrationInterface
{
private:
  const std::string getName() const { return name; }
  std::string name;

// HACK: should not be public, but inline access from StaticProvidingRegistrator
// needs
public:
  //HACK: remove it, here schouldn't be any object data
  T* data;

public:

  TypedRegistrationInterface(const std::string& name)
    : name(name),
      data(NULL)
  {
  }

  inline T& getData() const { assert(data != NULL); return *data; }
//  inline const T& getData() const { assert(data != NULL); return *data; }

  virtual Representation& registerAtBlackBoard(BlackBoard& blackBoard)
  {
    DataHolder<T>& rep = get(blackBoard);
    data = &(*rep);
    return rep;
  }

  virtual DataHolder<T>& get(BlackBoard& blackBoard)
  {
    return blackBoard.template getRepresentation<DataHolder<T> >(getName());
  }

  virtual const DataHolder<T>& get(const BlackBoard& blackBoard) const
  {
    return blackBoard.template getRepresentation<DataHolder<T> >(getName());
  }
};


/**
 *
 */
class Module: virtual public BlackBoardInterface
{
private:
  std::string moduleName;

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

  const list<Representation*>& getRequiredRepresentations()
  {
    return required;
  }

  const list<Representation*>& getProvidedRepresentations()
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
  static RepresentationMap* static_providing_registry;
  static RepresentationMap* static_requiring_registry;
  
  template<class TYPE_WHAT>
  class StaticRequiringRegistrator
  {
  private:
    TypedRegistrationInterface<TYPE_WHAT>* data;

  public:
    StaticRequiringRegistrator(){}

    StaticRequiringRegistrator(const std::string& name)
    {
      // TODO: check the type
      if(static_requiring_registry->find(name) == static_requiring_registry->end())
      {
        (*static_requiring_registry)[name] = new TypedRegistrationInterface<TYPE_WHAT>(name);
      }
      // HACK
      data = (TypedRegistrationInterface<TYPE_WHAT>*)(*static_requiring_registry)[name];
    }

    const TYPE_WHAT& get(const BlackBoard& blackBoard) const
    {
      return *(data->get(blackBoard));
    }
  };

  template<class TYPE_WHAT>
  class StaticProvidingRegistrator
  {
  private:
    TypedRegistrationInterface<TYPE_WHAT>* data;

  public:
    StaticProvidingRegistrator(){}

    StaticProvidingRegistrator(const std::string& name)
    {
      // TODO: check the type
      if(static_providing_registry->find(name) == static_providing_registry->end())
      {
        (*static_providing_registry)[name] = new TypedRegistrationInterface<TYPE_WHAT>(name);
      }
      // HACK
      data = (TypedRegistrationInterface<TYPE_WHAT>*)(*static_providing_registry)[name];
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
RepresentationMap* StaticRegistry<T>::static_providing_registry = new RepresentationMap();
template<class T>
RepresentationMap* StaticRegistry<T>::static_requiring_registry = new RepresentationMap();

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
  }the##representationName; \
  protected: \
  inline const representationName& get##representationName() const \
  { \
    static const representationName& representation = the##representationName.get(getBlackBoard()); \
    return representation; \
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
      registerRequiring(*static_requiring_registry); \
      registerProviding(*static_providing_registry); \
    } \
    virtual ~moduleName##Base(){ \
      unregisterRequiring(*static_requiring_registry); \
      unregisterProviding(*static_providing_registry); \
    } \
  };
   
#endif //__Module_h_
