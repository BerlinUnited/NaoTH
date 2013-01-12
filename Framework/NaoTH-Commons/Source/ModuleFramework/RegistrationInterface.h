


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
  {
  }

  Representation& registerAtBlackBoard(BlackBoard& blackBoard)
  {
    return blackBoard.template getRepresentation<DataHolder<T> >(getName());
  }
};//end class TypedRegistrationInterface


/**
* 
*/
class RegistrationInterfaceRegistry
{
private:
  RegistrationInterfaceMap provide_registry_map;
  RegistrationInterfaceMap require_registry_map;
public:
  RegistrationInterfaceMap& getProvide() { return provide_registry_map; }
  RegistrationInterfaceMap& getRequire() { return require_registry_map; }
};//end class RegistrationInterfaceRegistry