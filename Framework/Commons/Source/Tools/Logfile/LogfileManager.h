/** 
 * logging manager 
 * @author: thomas
 */

#ifndef _LogfileManager_h_
#define _LogfileManager_h_

#include <cstring>
#include <iostream>
#include <fstream>

#include <Tools/DataStructures/RingBuffer.h>

#include "LogfileEntry.h"

template<int maxSize> class LegacyLogfileManager
{
public:

  LegacyLogfileManager()
    :
    // NOTE: this was set to true in all usecases so far
    alwaysWriteOut(true),
    writtenBytes(0)
  {
    
  }

  /**
   * Constructor.
   *
   * @param alwaysWriteOut If false the ringbuffer is used. You have to call
   *                       flush() on your own.
   */
  /*
  LegacyLogfileManager(bool alwaysWriteOut)
    : 
    alwaysWriteOut(alwaysWriteOut),
    writtenBytes(0)
  {

  }
  */

  ~LegacyLogfileManager()
  {
    closeFile();
  }

  void openFile(const std::string& filePath)
  {
    closeFile();
    dataBuffer.clear();
    outFile.open(filePath, std::ios::out | std::ios::binary);
    writtenBytes = 0;
  }
  
  void closeFile()
  {
    if(outFile.is_open())
    {
      flush();
      outFile.close();
    }
  }


  /**
   * Create a new log entry.
   * After calling this use returned stringstream to transform the data.
   *
   */
  std::ostream& log(unsigned int frameNumber, const std::string& name)
  {
    if(alwaysWriteOut && dataBuffer.isFull())
    {
      flush();
      //cout << "flush before: " << frameNumber << endl;
    }

    // add empty entry
    dataBuffer.add();
    // fill with data
    LogfileEntry& newEntry = dataBuffer.getEntry(0);
    newEntry.frameNumber = frameNumber;
    newEntry.name = name;

    // the stream might not be empty, so reset it
    newEntry.data.rdbuf()->pubseekpos(0, std::ios::out);
    newEntry.data.str("");
    newEntry.data.clear();

    // make sure the stream is alright
    ASSERT(newEntry.data.good());

    return newEntry.data;
  }

  /**
  * Empty buffer and write to file
  */
  void flush()
  {
    if(outFile.is_open())
    {
      int length = dataBuffer.size();
      for (int i = length - 1; i >= 0; i--)
      {
        LogfileEntry& e = dataBuffer.getEntry(i);
        
        outFile.write((const char*)(&e.frameNumber), sizeof(unsigned int));
        writtenBytes += sizeof(unsigned int);

        outFile << e.name << '\0';
        writtenBytes += e.name.size() + 1;

        // DEBUG: make sure the data stream is alright
        //ASSERT(e.data.good());
        
        // size of data block
        long dataSize = (long)e.data.tellp(); 
        // NOTE: dataSize == -1 only in case of e.data.fail() == true
        //dataSize = dataSize < 0 ? 0 : dataSize;
        outFile.write((const char* ) &dataSize, 4);
        writtenBytes += 4;

        // NOTE: call of .str() involves making an additional copy of the data
        outFile.write((const char *) e.data.str().c_str(), dataSize);
        // "read" the whole content of e.data into outFile.rdbuf()
        // NOTE: this doesn't work with binary data, wil be fixed later
        //e.data.get(*outFile.rdbuf());

        // NOTE: this also happens when the memory is full, so we use it only for debug
        // crash if the file stream is broken
        /*
        if(!outFile.good()) {
          std::cout << "[LogfileManager] fail after writing " << e.name << std::endl;
          std::cout << "[LogfileManager] with error: " << std::strerror(errno);
          assert(false);
        }
        */

        writtenBytes += dataSize;
      }//end for

      outFile.flush();

      // clear buffer
      dataBuffer.clear();
    }//end if
  }//end flush

  size_t getWrittentBytesCount() const {
    return writtenBytes;
  }

  bool is_ready() const {
    return outFile.is_open() && !outFile.fail();
  }


private:  
  std::ofstream outFile;
  RingBuffer<LogfileEntry, maxSize> dataBuffer;
  bool alwaysWriteOut;
  size_t writtenBytes;
};

// NOTE: define the default LogfileManager type
//typedef LegacyLogfileManager<30> LogfileManager;

#include "DirectLogfileManager.h"
typedef DirectLogfileManager LogfileManager;

#endif //_LogfileManager_h_




