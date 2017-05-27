/** 
 * logging manager 
 * @author: thomas
 */

#ifndef _LogfileManager_h_
#define _LogfileManager_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include <Tools/DataStructures/RingBuffer.h>

#include "LogfileEntry.h"

using namespace std;

template<int maxSize> class LogfileManager
{
public:

  LogfileManager()
    : alwaysWriteOut(false)
  {
    
  }

  /**
   * Constructor.
   *
   * @param alwaysWriteOut If false the ringbuffer is used. You have to call
   *                       flush() on your own.
   */
  LogfileManager(bool alwaysWriteOut)
    : 
    alwaysWriteOut(alwaysWriteOut),
    writtenBytes(0)
  {

  }

  ~LogfileManager()
  {
    closeFile();
  }

  void openFile(const char* filePath)
  {
    closeFile();
    dataBuffer.clear();
    outFile.open(filePath, ios::out | ios::binary);
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
  stringstream& log(unsigned int frameNumber, string name)
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
    newEntry.data.str("");
    newEntry.data.clear();

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

        // size of data block
   //     size_t dataSize = e.data.str().size();
        long dataSize = (long)e.data.tellp(); dataSize = dataSize < 0 ? 0 : dataSize;
        outFile.write((const char* ) &dataSize, 4);
        writtenBytes += 4;

        // the data itself
        outFile.write((const char *) e.data.str().c_str(), dataSize);
        //outFile << e.data.rdbuf();
        writtenBytes += dataSize;
        
        // clear string buffer
        e.data.clear();
        e.data.str("");
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
  ofstream outFile;
  RingBuffer<LogfileEntry, maxSize> dataBuffer;
  bool alwaysWriteOut;
  size_t writtenBytes;
};

#endif //_LogfileManager_h_




