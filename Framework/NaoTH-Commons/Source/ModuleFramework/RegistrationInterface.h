


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
public:
  RegistrationInterfaceMap registry;

  template<class R>
  RegistrationInterface* registerInterface(const std::string& name)
  {
    RegistrationInterfaceMap::iterator i = registry.find(name);
    if(i == registry.end())
      i = registry.insert(registry.begin(), std::make_pair(name, new TypedRegistrationInterface<R>(name)));

    return i->second;
  }
};
