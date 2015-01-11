/**
* @file Singleton.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration and definition of class Singleton
*
* Usage:
*  Case 1: the static instanse is created on the stack by default

   class MyClass: public Singleton<MyClass>
   {
   private:
     friend class Singleton<MyClass>; // allow Singleton access to the private constructor
     MyClass(){} // hide constructor
      ...
   };

* this is equivalent to following

   class MyClass: public Singleton<MyClass, _SINGLETON_USE_STACK>
   {
      ...
   };

*  Case 2: the static instanse is created on the heap

   class MyClass: public Singleton<MyClass, _SINGLETON_USE_HEAP>
   {
      ...
   };
*/

#ifndef _Singleton_h_
#define _Singleton_h_

#include <stdlib.h>

#define _SINGLETON_USE_STACK 0
#define _SINGLETON_USE_HEAP 1

namespace naoth 
{

template<class V, int storageType=_SINGLETON_USE_HEAP>
class Singleton
{
};


template<class V>
class Singleton<V,_SINGLETON_USE_STACK>
{
protected:
  Singleton(){}
  Singleton( const Singleton& ){}
public:
  virtual ~Singleton(){}
  static V& getInstance()
  {
    static V theInstance;
    return theInstance;
  }
};

template<class V>
class Singleton<V,_SINGLETON_USE_HEAP>
{
protected:
  Singleton(){}
  Singleton( const Singleton& ){}
public:
  virtual ~Singleton(){}

  static V& getInstance()
  {
    static Watcher w;
    return *w.theInstance;
  }
private:
  class Watcher {
  public:
    Watcher() {
      theInstance = new V();
    }
    ~Watcher() {
      delete theInstance;
    }
    V* theInstance;
  };
};

}//end namespace naoth

#endif //_Singleton_h_
