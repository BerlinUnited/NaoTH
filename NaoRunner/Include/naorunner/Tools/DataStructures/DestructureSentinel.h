/**
* @file DestructionSentinel.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration and definition of classes DestructionSentinel and DestructionListener.
* This classes implements a possibility to watch if a object is destroyed using a 
* listener concept. Example of usage:
  
  class A: public DestructionSentinel<A>
  {
  public:
    std::string name;

    A(std::string name)
    {
      this->name = name;
    }
  };


  class B: public DestructionListener<A>
  {
  public:
    virtual void objectDestructed(A* object)
    {
       std::cout << "object " << object->name << " destroyed" << endl;
    }

    void test()
    {
      A foo("foo");
      A bar("bar");
      foo.registerDestructionListener(*this);
      bar.registerDestructionListener(*this);
    }
  };
*
*/
#ifndef __DestructionSentinel_h_
#define __DestructionSentinel_h_

#include <list>

namespace naorunner 
{

  template<class T> class DestructionSentinel;
  template<class T> class DestructionListener;

  /**
   * class DestructionListener<T>
   * Parent class for an observer. The virtual method objectDestructed should be overwritten
   * by the watching class.
   * If/When the destructor of an observed object is executed, this method will be called witch
   * the pointer to this object. Type T should be the class-type of the observed object.
   */
  template<class T>
  class DestructionListener
  {
    friend class DestructionSentinel<T>;
    std::list<DestructionSentinel<T>*> listOfDestructionSentinel;

    void registerDestructionSentinel(DestructionSentinel<T>& sentinel)
    {
      listOfDestructionSentinel.push_back(&sentinel);
    }//end registerDestructionSentinel

    void sentinelDestructed(DestructionSentinel<T>* object)
    {
      listOfDestructionSentinel.remove(object);
      objectDestructed((T*)object); // execute custom destruction action
    }//end sentinelDestructed

  protected:
    virtual ~DestructionListener()
    {
      typename std::list<DestructionSentinel<T>*>::iterator iter;
      for(iter = listOfDestructionSentinel.begin(); iter != listOfDestructionSentinel.end(); iter++)
      {
        (*iter)->listenerDestructed(this);
      }//end for
    }//end ~DestructionListener

  public:
    virtual void objectDestructed(T* object) = 0;
  };//end class DestructionListener


  /**
   * class DestructionSentinel<T>
   * Parent class for an observed object. The observers can be registered using the method
   * registerDestructionListener. When the destructor of the DestructionSentinel is called, all
   * the registered listeners/observers are notified.
   */
  template<class T>
  class DestructionSentinel
  {
    friend class DestructionListener<T>;
    std::list<DestructionListener<T>*> listOfDestructionListener;

    void listenerDestructed(DestructionListener<T>* object)
    {
      listOfDestructionListener.remove(object);
    }//end listenerDestructed

  protected:
    ~DestructionSentinel()
    {
      typename std::list<DestructionListener<T>*>::iterator iter;
      for(iter = listOfDestructionListener.begin(); iter != listOfDestructionListener.end(); iter++)
      {
        (*iter)->sentinelDestructed(this);
      }//end for
    }//end ~DestructionSentinel

  public:
    void registerDestructionListener(DestructionListener<T>& listener)
    {
      listOfDestructionListener.push_back(&listener);
      listener.registerDestructionSentinel(*this);
    }//end registerDestructionListener
  };//end class DestructionSentinel
}

#endif /* __DestructionSentinel_h_ */
