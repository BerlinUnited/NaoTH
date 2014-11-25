/** 
* @file XabslArray.h
*
* Declaration and implementation of template class NamedArray
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias J�ngel</a>
* @author <a href="http://www.tzi.de/~roefer/">Thomas R�fer</a>
*/


#ifndef __XabslArray_h_
#define __XabslArray_h_

#include <stdlib.h>
#include <string.h>

namespace xabsl 
{

/**
* @class NamedItem
* A class that has a text label
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
*/
class NamedItem
{
public:
  /** 
  * Constructor
  * @param name The name of the item
  */
  NamedItem(const char* name)
  {
      n = new char[strlen(name)+1];
      strcpy(n,name);
  }

  /** Destructor. Deletes the name */
  virtual ~NamedItem() { delete[] n; }

  /** The text label */
  char* n;
};

/**
* An element of an NamedArray.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias J�ngel</a>
* @author <a href="http://www.tzi.de/~roefer/">Thomas R�fer</a>
*/
template<class T> class NamedArrayElement : public NamedItem
{
  public:
    /**
    * Constructor.
    * @param name A string label for the element.
    * @param element The new element.
    */
    NamedArrayElement(const char* name, T element)
      : NamedItem(name), e(element) {}
    
    /** 
    * Destructor. If the element is a pointer, it has to be 
    * deleted externally 
    */
    virtual ~NamedArrayElement() {}
    
    /** The element */
    T e;
};

/**
* The class implements a dynamic array.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias J�ngel</a>
* @author <a href="http://www.tzi.de/~roefer/">Thomas R�fer</a>
* @author <a href="http://www.sim.informatik.tu-darmstadt.de/pers/card/risler.html">Max Risler</a>
*/
template <class T> class Array
{
  public:
    /** Constructor */
    Array() 
    {
      usedSize = 0;
      allocatedSize = 2;
      data = new T[allocatedSize];
    }
    
    /** Destructor */
    virtual ~Array() 
    { 
      delete[] data;
    }

    /** Assignment operator
    *\param other The other Array that is assigned to this one
    *\return A reference to this object after the assignment.
    */
    Array<T>& operator=(const Array<T>& other)
    {
      delete[] data;
      usedSize = other.usedSize;
      allocatedSize = other.allocatedSize;
      data = new T[allocatedSize];
      for (int i = 0; i < usedSize; i++)
        data[i]=other.data[i];
      return *this;
    }

    /** Copy constructor
    *\param other The other vector that is copied to this one
    */
    Array(const Array<T>& other) 
    {
      data = new T[0];
      *this = other;
    }

    /** Clears the array */
    void clear()
    {
      delete[] data;
      usedSize = 0;
      allocatedSize = 2;
      data = new T[allocatedSize];
    }
        
    /** 
    * Returns the value for a given array position.
    * Note that the function crashes if the required position is bigger than the 
    * size of the array.
    */
    T getElement(int pos) const
    {
      return data[pos];
    }
    
    /**
    * The function appends a new element to the array.
    * @param element The new element.
    */
    void append(T element)
    {
      if(usedSize == allocatedSize)
      {
        allocatedSize += allocatedSize / 2; // note that allocatedSize must be at least 2
        T* temp = new T[allocatedSize];
        for(int i = 0; i < getSize(); ++i)
          temp[i] = data[i];
        delete[] data;
        data = temp;
      }
      data[usedSize++] = element;
    }
    
    /**
    * The function sets the value of an element in the array.
    * Note that the function crashes if the element does not exist.
    * @param pos The position of the element in the array.
    * @param value The new element.
    */
    void setElement(int pos, T value)
    {
      data[pos] = value;
    }

    /**
    * The function returns the number of elements in the array.
    * @return The length of the list.
    */
    int getSize() const {return usedSize;}
    
    /** 
    * Returns the value for a given array position.
    * Note that the function crashes if the required position is bigger than the 
    * size of the array.
    */
    const T& operator[](int pos) const
    {
      return data[pos];
    }
    T& operator[](int pos)
    {
      while (pos >= usedSize) append(T());
      return data[pos];
    }
    
    /** Removes the last element of the array */
    void removeLast()
    {
      if (usedSize > 0) 
        usedSize--;
    }

    // TODO: is this operator needed??
//    operator Array<const T>&() {return this;}

  protected:
    /** The array */
    T* data;

    /** The number of elements in the array */
    int usedSize, allocatedSize;
};

/**
* The class implements a dynamic array. Each array element can have a text label.
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.informatik.hu-berlin.de/~juengel">Matthias J�ngel</a>
* @author <a href="http://www.tzi.de/~roefer/">Thomas R�fer</a>
*/
template <class T> class NamedArray : public Array<NamedArrayElement<T>*>
{
  public:
    /** Destructor */
    virtual ~NamedArray()
    {
      for(int i = 0; i < this->getSize(); ++i)
        delete this->data[i];
    }

    /** Assignment operator
    *\param other The other NamedArray that is assigned to this one
    *\return A reference to this object after the assignment.
    */
    NamedArray<T>& operator=(const NamedArray<T>& other)
    {
      for(int i = 0; i < this->usedSize; ++i)
        delete this->data[i];
      delete[] this->data;
      this->usedSize = other.usedSize;
      this->allocatedSize = other.allocatedSize;
      this->data = new T[this->allocatedSize];
      for (int i = 0; i < this->usedSize; i++)
        this->data[i]=new NamedArrayElement<T>(other.data[i]->n,other.data[i]->e);
      return *this;
    }

    /** Clears the array */
    void clear()
    {
      for(int i = 0; i < this->getSize(); ++i)
        delete this->data[i];
      Array<NamedArrayElement<T>*>::clear();
    }

    /** 
    * Returns the value for a given name. 
    * If no element exists for the name, the default value is returned.
    * @param name The name element
    * @param defaultValue The value that is returned if no element exists for the name.
    * @return Either the element found or the default value. 
    */
    T getElement(const char* name, T defaultValue) const
    {
      int pos = find(name);
      if(pos < 0)
        return defaultValue;
      else
        return getElement(pos)->e;
    }
    
    /** 
    * Returns the value for a given name. 
    * Note that the function crashes if the element does not exist.
    * @param name The name of the element
    */
    T& getElement(const char* name) const
    {
      return getElement(find(name));
    }

    /** 
    * Returns the value for a given array position.
    * Note that the function crashes if the required position is bigger than the 
    * size of the array.
    */
    T& getElement(int pos) const
    {
      return this->data[pos]->e;
    }
    
    /** 
    * Returns a pointer to the array element for a given name.
    * Note that the function crashes if the element does not exist
    * @param name the name of the element 
    */
    NamedArrayElement<T>* getPElement(const char* name)
    {
      return this->data[find(name)];
    }

    /** Returns the name of an element */
    const char* getName(int pos) const
    {
      return this->data[pos]->n;
    }

    /**
    * The function appends a new element to the array.
    * @param name A string label for the element.
    * @param element The new element.
    */
    void append(const char* name, T element)
    {
      Array<NamedArrayElement<T>*>::append(new NamedArrayElement<T>(name,element));
    }
    void append(T element)
    {
      append("",element);
    }
    
    /**
    * The function sets the value of an element in the array.
    * Note that the function crashes if the element does not exist.
    * @param name A string label for the element.
    * @param value The new element.
    */
    void setElement(const char* name, T value)
    {
      setElement(find(name),value);
    }

    /**
    * The function sets the value of an element in the array.
    * Note that the function crashes if the element does not exist.
    * @param pos The position of the element in the array.
    * @param value The new element.
    */
    void setElement(int pos, T value)
    {
      this->data[pos]->e = value;
    }
    
    /** 
    * Returns the value for a given array position.
    * Note that the function crashes if the required position is bigger than the 
    * size of the array.
    */
    T operator[](int pos) const
    {
      return getElement(pos);
    }
    
    /** 
    * Returns the value for a given name. 
    * Note that the function crashes if the element does not exist.
    * @param name The name of the element
    */
    T operator[](const char* name) const
    {
      return getElement(name);
    }

    /** Returns whether an element for the given name exists */
    bool exists(const char* name) const
    {
      return find(name) >= 0;
    }

    /** Removes the last element of the array */
    void removeLast()
    {
      if (this->usedSize > 0)
        delete this->data[--(this->usedSize)];
    }

  protected:
    /** 
    * Returns the index of an element with the given name.
    * @param name The name that is searched for.
    * @return The index of the element of -1 if the name does not exist.
    */
    int find(const char* name) const
    {
      for(int i = 0; i < this->getSize(); ++i)
        if(!strcmp(getName(i),name))
          return i;
      return -1;
    }
};


} // namespace

#endif // __XabslArray_h_






