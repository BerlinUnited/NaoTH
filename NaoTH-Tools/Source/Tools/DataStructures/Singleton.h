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

#ifndef __Singleton_h__
#define __Singleton_h__

#include <stdlib.h>

#define _SINGLETON_USE_STACK 0
#define _SINGLETON_USE_HEAP 1

namespace naoth 
{

  template<class V, typename ReturnType=V, int storageType=_SINGLETON_USE_STACK>
  class Singleton
  {
  protected:
    static V* theInstance;
    Singleton(){}

  public:

    static ReturnType& getInstance()
    {
      if(theInstance == NULL)
      {
        theInstance = createInstance<storageType>();
      }//end if

      return *theInstance;
    }//end getInstance

  private:

    template<int ST>
    static V* createInstance()
    {
      // dummy/declaration
      // TODO: assert?
    }

    // creates an instance on the heap
    // (only the Watcher is created on the stack)
    template<>
    static V* createInstance<_SINGLETON_USE_HEAP>()
    {
      static Watcher w; // create a watcher
      if( theInstance == NULL)
        theInstance = new V();
      return theInstance;
    }//end createInstanceOnHeap

    // creates an instance on the stack
    template<>
    static V* createInstance<_SINGLETON_USE_STACK>()
    {
      static V theInstance; // create a watcher
      return &theInstance;
    }//end createInstanceOnStack

    Singleton( const Singleton& ){};

    static void deleteInstance()
    {
      if( Singleton::theInstance != 0 )
          delete Singleton::theInstance;
    }//end deleteInstance

    class Watcher {
      public: ~Watcher() {
        Singleton::deleteInstance();
      }
    };
    friend class Watcher;
  };

  template <class V, typename ReturnType, int storageType>
  V* Singleton<V,ReturnType,storageType>::theInstance = NULL;
}

#endif //__Singleton_h__
