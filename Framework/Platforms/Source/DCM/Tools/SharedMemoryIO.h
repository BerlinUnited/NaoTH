/**
* @file SharedMemoryIO.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinric Mellmann</a>
* @breief provides a convenient interface for the shared memory
*
*/

#include <glib.h>
#include "SharedMemory.h"

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
  SharedMemoryWriter() : mutex(NULL)
  {
    mutex = g_mutex_new();
  }

  ~SharedMemoryWriter()
  {
    g_mutex_free(mutex);
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
    g_mutex_lock(mutex);
    // copy the data into the shared memory
    memory.writing()->set(data);
    memory.swapWriting();
    g_mutex_unlock(mutex);
  }

private:
 GMutex* mutex;
 SharedMemory<V> memory;
};//end class SharedMemoryWriter


template<class V>
class SharedMemoryReader
{
public:
  SharedMemoryReader() : mutex(NULL)
  {
    mutex = g_mutex_new();
  }

  ~SharedMemoryReader()
  {
    g_mutex_free(mutex);
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
    g_mutex_lock(mutex);
    memory.reading()->get(data);
    g_mutex_unlock(mutex);
  }

  // delegate
  inline bool swapReading(){ return memory.swapReading(); }
  inline const V& data() { return *memory.reading(); }

private:
 GMutex* mutex;
 SharedMemory<V> memory;
};//end class SharedMemoryReader

} // end naoth