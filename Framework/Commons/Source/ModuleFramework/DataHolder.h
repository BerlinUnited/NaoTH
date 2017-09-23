/**
* @file DataHolder.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
* 
*/

#ifndef _DataHolder_h_
#define _DataHolder_h_

#include "Representation.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include <type_traits>

namespace naoth
{
template<>
class Serializer<Representation>
{
public:
  static void serialize(const Representation& representation, std::ostream& stream) {
    representation.serialize(stream);
  }
  static void deserialize(std::istream& stream, Representation& representation) {
    representation.deserialize(stream);
  }
};
}

/**
 * Connects an arbitrary class with Representation
 */
template<class T>
class DataHolder: public Representation, public T
{
private:
  // creates an object of the data
  // (this requires the class T to have a default constructor)
  //T  data;

public:
  DataHolder(const std::string& name): Representation(name){}
  DataHolder(): Representation(typeid(T).name()){}
  virtual ~DataHolder(){}

  T& operator*() { return *this; }
  const T& operator*() const { return *this; }

  /** 
   * wrap the print, fromDataStream and toDataStream of the data member 
   */
  virtual void print(std::ostream& stream) const {
    std::conditional<std::is_base_of<Printable,T>::value, T, Representation>::type::print(stream);
  }

  virtual bool serializable() const {
    return !std::is_base_of<naoth::EmptySerializer,naoth::Serializer<T> >::value;
  }

  void serialize(MsgOut<Representation>::type& msg) const {
    naoth::Serializer<T>::serialize(*this, msg);
  }

  void deserialize(MsgIn<Representation>::type& msg) {
    naoth::Serializer<T>::deserialize(msg, *this);
  }
};


#endif  /* _DataHolder_h_ */

