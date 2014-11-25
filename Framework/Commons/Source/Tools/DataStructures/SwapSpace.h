/**
 * @file SwapSpace.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Mellmann, Heinrich</a>
 * @breief 1 to N swap space
 *
 */

#include <string>
#include <vector>
#include <queue>

#ifndef _SwapSpace_h_
#define _SwapSpace_h_

template<typename T>
class SwapSpace
{
private:

  class CountRef
  {
  public:
      T* ptr;
      int count;

    CountRef()
      : 
      ptr(NULL),
      count(0)
    {}

    CountRef(T* ptr)
      : 
      ptr(ptr),
      count(0)
    {}
  };// end CountRef

private:
  class Memory
  {
  public:
    volatile bool new_data_avaliable;
    volatile T* writing, swapping;
    CountRef reading;
    std::queue<CountRef*> free_slots;
    std::vector<CountRef> avaliable_slots;

    T write_memory;
    T swap_memory;

    Memory()
      : 
      new_data_avaliable(false),
      writing(NULL),
      reading(NULL)
    {
      avaliable_slots.push_back(CountRef(&write_memory));
      avaliable_slots.push_back(CountRef(&swap_memory));
    }
  };//end Memory


public:
  class ReadAccessor
  {
  private:
    volatile CountRef* reading;
    Memory& memory;
  public:
    ReadAccessor(Memory& memory)
      : reading(NULL),
        memory(memory)
    {
      init();
    }

    bool init()
    {
      if ( !lock() )
        return false;

      // free the current slot
      if(reading && --reading.count == 0)
      {
        memory.free_slots.push(reading);
      }

      // check if new data is avaliable
      const bool new_data_avaliable = memory.new_data_avaliable;
      if ( new_data_avaliable )
      {
        // get new free slot
        memory.reading = memory.free_slots.front();
        memory.free_slots.pop();
        swap(memory.reading.ptr, memory.swapping);
        memory.new_data_avaliable = false;
      }

      // take the newest data
      reading = memory.reading;
      reading.count++;

      unlock();
      return memory.new_data_avaliable;
    }//end init

    T& operator* ()
    {
      return *reading.ptr;
    }
  };//end ReadAccessor


public:
  SwapSpace()
  {
  }
  

  ~SwapSpace()
  {
  }

  ReadAccessor* add_reader(T& slot)
  {
    theMemory.avaliable_slots.push_back(CountRef(&slot));
    theMemory.free_slots.push(&theMemory.avaliable_slots.back());

    // TODO: treat this memory leak
    return new ReadAccessor(theMemory); 
  }//end add_reader
  

  void swapWriting()
  {
    if ( lock() )
    {
      swap(theMemory.writing, theMemory.swapping);
      theMemory.new_data_avaliable = true;
    }
    unlock();
  }//end swapWriting

  //T* writing() { return writing; }
  //const T* reading() const { return reading; }
  
    
protected:
    bool trylock() { /*return sem_trywait(sem) == 0;*/ }
    void lock() { /*sem_wait(sem);*/ }
    void unlock() { /*sem_post(sem);*/ }
  
private:
  Memory theMemory;
};

#endif // _SwapSpace_h_
