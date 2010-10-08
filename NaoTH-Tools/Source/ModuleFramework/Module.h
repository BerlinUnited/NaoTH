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
#include "Node.h"

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
  const std::string getName() const { return typeid(T).name(); }

public:
  virtual Representation& registerAtBlackBoard(BlackBoard& blackBoard)
  {
    return blackBoard.template getRepresentation<DataHolder<T> >(getName());
  }
};


/**
 *
 */
class Module: virtual public BlackBoardInterface
{
protected:
  const std::string moduleName;

  // pointers to the provided and required representations
  Node<Representation, Representation> node;


  Module(std::string name): moduleName(name)
  {
    // std::cout << "Load " << getModuleName() << endl;
  }

  Module(): moduleName("invalide module")
  {
    // should never be here
  }

  // TODO: remove, make it tool methods
  void registerUsing(const RepresentationMap& list);
  void registerProviding(const RepresentationMap& list);
  void unregisterUsing(const RepresentationMap& list);
  void unregisterProviding(const RepresentationMap& list);

public:

  const list<Representation*>& getUsedRepresentations()
  {
    return node.from;
  }

  const list<Representation*>& getProvidedRepresentations()
  {
    return node.to;
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
  static RepresentationMap* static_using_registry;

  template<class TYPE_WHAT>
  class StaticUsingRegistrator
  {
  public:
    StaticUsingRegistrator()
    {
      // HACK: no variable names possible
      std::string name = typeid(TYPE_WHAT).name();
      // TODO: check the type
      if(static_using_registry->find(name) == static_using_registry->end())
      {
        (*static_using_registry)[name] = new TypedRegistrationInterface<TYPE_WHAT>();
      }
    }
  };

  template<class TYPE_WHAT>
  class StaticProvidingRegistrator
  {
  public:
    StaticProvidingRegistrator()
    {
      // HACK: no variable names possible
      std::string name = typeid(TYPE_WHAT).name();
      // TODO: check the type
      if(static_providing_registry->find(name) == static_providing_registry->end())
      {
        (*static_providing_registry)[name] = new TypedRegistrationInterface<TYPE_WHAT>();
      }
    }
  };
};

template<class T>
RepresentationMap* StaticRegistry<T>::static_providing_registry = new RepresentationMap();
template<class T>
RepresentationMap* StaticRegistry<T>::static_using_registry = new RepresentationMap();


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
  StaticUsingRegistrator<representationName> _the##representationName; \
  protected: \
  const representationName& get##representationName() \
  { \
    static const DataHolder<representationName>& representation = getBlackBoard().template getConstRepresentation<DataHolder<representationName> >(typeid(representationName).name()); \
    return *representation; \
  }

#define PROVIDE(representationName) \
  private: \
  StaticProvidingRegistrator<representationName> _the##representationName; \
  protected: \
  representationName& get##representationName() \
  { \
    static DataHolder<representationName>& representation = getBlackBoard().template getRepresentation<DataHolder<representationName> >(typeid(representationName).name()); \
    return *representation; \
  }


#define END_DECLARE_MODULE(moduleName) \
  public: \
    moduleName##Base(): Module(#moduleName){ \
      registerUsing(*static_using_registry); \
      registerProviding(*static_providing_registry); \
    } \
    ~moduleName##Base(){ \
      unregisterUsing(*static_using_registry); \
      unregisterProviding(*static_providing_registry); \
    } \
  };
 
#endif //__Module_h_
