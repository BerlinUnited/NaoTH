


#include "BlackBoard.h"
#include "DataHolder.h"

/**
* 
*/
class RegistrationInterface
{
private:
  std::string name;

public:
  RegistrationInterface(const std::string& name) : name(name){}
  virtual ~RegistrationInterface(){}
  const std::string getName() const { return name; }

  virtual Representation& registerAtBlackBoard(BlackBoard& blackBoard) = 0;
};

/** type for a named map of representation interfaces */
typedef std::map<std::string, RegistrationInterface*> RegistrationInterfaceMap;

/**
*
*/
template<class T>
class TypedRegistrationInterface: public RegistrationInterface
{
public:

  TypedRegistrationInterface(const std::string& name)
    : RegistrationInterface(name)
  {}

  Representation& registerAtBlackBoard(BlackBoard& blackBoard)
  {
    return blackBoard.template getRepresentation<DataHolder<T> >(getName());
  }
};//end class TypedRegistrationInterface


/**
*/
class RegistrationInterfaceRegistry
{
private:
  RegistrationInterfaceMap registry_map;

public:
  ~RegistrationInterfaceRegistry() {
    for(RegistrationInterfaceMap::iterator i = registry_map.begin(); i != registry_map.end(); ++i) {
      delete i->second;
    }
  }

  template<class R>
  RegistrationInterface* registerInterface(const std::string& name)
  {
    RegistrationInterfaceMap::iterator i = registry_map.find(name);
    if(i == registry_map.end()) {
      i = registry_map.insert(registry_map.begin(), std::make_pair(name, new TypedRegistrationInterface<R>(name)));
    }

    return i->second;
  }

  inline const RegistrationInterfaceMap& registry() { return registry_map; }
};
