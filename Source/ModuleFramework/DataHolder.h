/* 
 * File:   DataHolder.h
 * Author: thomas
 *
 * Created on 8. Oktober 2010, 12:12
 */

#ifndef __DataHolder_h_
#define __DataHolder_h_

#include "Representation.h"
#include "Tools/Printable.h"
#include "Tools/Serializer.h"


namespace crf
{

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
  virtual void print(ostream& stream) const
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
  /*
  void serialize(std::ostream& stream) const
  {
    Serializer<T>::serialize(data, stream);
  }

  void deserialize(std::istream& stream)
  {
    Serializer<T>::deserialize(stream, data);
  }
  */
  void serialize(crf::BaseMessage& msg) const
  {
    Serializer<T>::serialize(data, msg);
  }

  void deserialize(crf::BaseMessage& msg)
  {
    Serializer<T>::deserialize(msg, data);
  }
};

}//crf

#endif  /* __DataHolder_h_ */

