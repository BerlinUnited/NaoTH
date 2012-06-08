/**
 * @file SharedMemory.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinric Mellmann</a>
 * @breief Shared memory wrapper
 *
 */

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>            /* For perror */
#include <semaphore.h>
#include <cerrno>
#include <string>

#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

template<typename T>
class SharedMemory
{
private:
  class Memory
  {
  public:
    Memory():writing(0),swapping(1),reading(2),swappingReady(false){}
  
    int numRef; // how many objects share the same memory
    volatile int writing, swapping, reading;
    volatile bool swappingReady;
    
    T data[3]; // buffers, one for writing, on for reading, and one for swapping
  }; // end class Memory
  
public:
  SharedMemory()
    :shmId(-1),
     sem(SEM_FAILED),
     theMemory((Memory*)MAP_FAILED),
     ready(false)
     {
     }
    
  bool open(const std::string& name)
  {
    close();
    
    theName = name;
    
      if((sem = sem_open(theName.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED)
      {
        perror("can not open semaphore");
        return false;
      }

      trylock();
      
      if((shmId = shm_open(theName.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
      {
        perror("can not open shared memory");
        return false;
      }

      struct stat shmStat;
      if ( fstat(shmId, &shmStat) != 0 )
      {
        perror("can not stat shared memory");
        return false;
      }

      const int memSize = sizeof(Memory);
      bool newMemory = false;
      
      if ( shmStat.st_size == 0 )
      {
        newMemory = true;
        // allocate memory
        if(ftruncate(shmId, memSize) != 0)
        {
          perror("can not truncate shared memory");
          return false;
        }
      }
      else if ( shmStat.st_size != memSize )
      {
        fprintf(stderr, "memory size are different\n");
        return false;
      }

      // map data      
      if((theMemory = (Memory*)mmap(NULL, memSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmId, 0)) == MAP_FAILED)
      {
        perror("can not map shared memory");
        return false;
      }

      if (newMemory)
        theMemory->numRef = 1;
      else
        theMemory->numRef++;

      unlock();
      
      ready = true;
      
      return ready;
    }

  ~SharedMemory()
  {
    close();
  }
  
  // return if get the new data
  bool swapReading()
  {
    if ( !trylock() )
      return false;

    const bool swappingReady = theMemory->swappingReady;
    if ( swappingReady )
    {
      swap(theMemory->reading, theMemory->swapping);
      theMemory->swappingReady = false;
    }
    unlock();
    return swappingReady;
  }//end swapReading
  

  void swapWriting()
  {
    if ( trylock() )
    {
      swap(theMemory->writing, theMemory->swapping);
      theMemory->swappingReady = true;
    }
    unlock();
  }//end swapWriting


  T* writing() { return &(theMemory->data[theMemory->writing]); }
  
  const T* reading() const { return &(theMemory->data[theMemory->reading]); }
  
  void close()
  {
    if ( !ready ) return;
      
    lock();
    theMemory->numRef--;
    if ( theMemory->numRef == 0 )
    {
      shm_unlink(theName.c_str());
      sem_unlink(theName.c_str());
    }
    else
    {
      unlock();
    }
      
    ready = false;
  }
    
protected:
    bool trylock() { return sem_trywait(sem) == 0; }
    
    void lock()
    {
      if(sem_wait(sem) == -1)
      {
        std::cerr << "lock errno: " << errno << std::endl;
      }
    }//end lock

    void unlock() 
    {
      int sval;
      if(sem_getvalue(sem, &sval) == 0)
      {
        if(sval < 1)
        {
          sem_post(sem); 
        }
      }
    }//end unlock

private:
  std::string theName;
  int shmId;
  sem_t* sem;
  Memory* theMemory;
  bool ready;
};

#endif
