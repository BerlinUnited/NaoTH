


#include "BlackBoard.h"
#include "DataHolder.h"

/**
 * 
 */
class RegistrationInterface
{
public:
  virtual Representation& registerAtBlackBoard(BlackBoard& blackBoard) = 0;
};



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

//  inline T& getData() const { assert(data != NULL); return *data; }
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