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


namespace naoth
{
  template<>
  class Serializer<Representation>
  {
  public:
    static void serialize(const Representation& representation, std::ostream& stream)
    {
      representation.serialize(stream);
    }
    static void deserialize(std::istream& stream, Representation& representation)
    {
      representation.deserialize(stream);
    }
  };
}

/**
 * Connects a arbitrary class with Representation
 */
template<class T>
class DataHolder: public Representation
{
private:
  // creates an object of the data
  // (this requires the class T to have a default constructor)
  T data;

public:
  DataHolder(const std::string& name) : Representation(name){}
  DataHolder() : Representation(typeid(T).name()){}
  virtual ~DataHolder(){}

  T& operator*(){ return data; }
  const T& operator*() const{ return data; }

  /** 
   * wrap the print, fromDataStream and toDataStream of the data member 
   */
  virtual void print(std::ostream& stream) const
  {
    const T* asPointer =&data;
    const Printable* asPrintable = dynamic_cast<const Printable*>(asPointer);
    if(asPrintable != NULL)
    {
      stream << *asPrintable;
    }
    else
    {
      // use representation by default
      stream << getName();
    }
  }//end print

  void serialize(std::ostream& stream) const
  {
    Serializer<T>::serialize(data, stream);
  }

  void deserialize(std::istream& stream)
  {
    Serializer<T>::deserialize(stream, data);
  }
};


#endif  /* _DataHolder_h_ */

