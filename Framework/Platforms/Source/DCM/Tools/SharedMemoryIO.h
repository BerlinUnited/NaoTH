/**
* @file SharedMemoryIO.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinric Mellmann</a>
* @breif provides a convenient interface for the shared memory
*
*/
#ifndef _SharedMemoryIO_h
#define _SharedMemoryIO_h

#include "SharedMemory.h"

#include <mutex>

namespace naoth
{

/**
* 
*/
template<class T>
class Accessor
{
public:
  inline void set(const T& other) { data = other; }
  inline void get(T& other) { other = data; }
  inline const T& get() const { return data; }
private:
  T data;
};

// MutexSharedMemoryReaderWithSwap
template<class V>
class SharedMemoryWriter
{
public:
  SharedMemoryWriter()
  {
  }

  ~SharedMemoryWriter()
  {
    memory.close();
  }

  void open(std::string name)
  {
    // debug
    std::cout << "Opening shared memory (write): " << name << std::endl;
    if(!memory.open(name)) {
      assert(false);
    }
  }

  template<class T>
  void set(const T& data)
  { 
    std::lock_guard<std::mutex> lock(_mutex);
    // copy the data into the shared memory
    memory.writing()->set(data);
    memory.swapWriting();
  }

private:
 std::mutex _mutex;
 SharedMemory<V> memory;
};//end class SharedMemoryWriter


template<class V>
class SharedMemoryReader
{
public:
  SharedMemoryReader()
  {
  }

  ~SharedMemoryReader()
  {
    memory.close();
  }

  void open(std::string name)
  {
    // debug
    std::cout << "Opening shared memory (read): " << name << std::endl;
    if(!memory.open(name)) {
      assert(false);
    }
  }

  template<class T>
  void get(T& data) const 
  { 
    std::lock_guard<std::mutex> lock(_mutex);
    memory.reading()->get(data);
  }

  // delegate
  inline bool swapReading(){ return memory.swapReading(); }
  inline const V& data() { return *memory.reading(); }

private:
 mutable std::mutex _mutex;
 SharedMemory<V> memory;
};//end class SharedMemoryReader

} // end naoth

#endif // _SharedMemoryIO_h
