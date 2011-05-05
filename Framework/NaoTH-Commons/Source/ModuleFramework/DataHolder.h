/* 
 * File:   DataHolder.h
 * Author: thomas
 *
 * Created on 8. Oktober 2010, 12:12
 */

#ifndef DATAHOLDER_H
#define	DATAHOLDER_H

#include "Representation.h"
#include "Tools/DataStructures/Printable.h"

using namespace naoth;

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
};


#endif	/* DATAHOLDER_H */

