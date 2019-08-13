/** 
 * logging manager 
 * @author: Heinrich
 */

#ifndef _DirectLogfileManager_h_
#define _DirectLogfileManager_h_

#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Tools/Debug/NaoTHAssert.h"

class DirectLogfileManager
{
public:

  class SimpleBuffer :public std::streambuf
  {
  public:
    SimpleBuffer() : position (0)
    {}
  
    const size_t& size() const {
      return position;
    }
  
    void clear() {
      position = 0;
    }
  
    const char* buf() const {
      return buffer;
    }

  protected:
      // copy the given buffer into the accumulated fast buffer
      inline std::streamsize xsputn(const char* s, std::streamsize count) override
      {
        // std::streamsize can be negative (in which cases?)
        assert(count > 0);
        // the message is longer than the buffer, this should never happen
        assert(position + count < length);
        std::memcpy(&buffer[position], s, (size_t)count);
        position += (size_t)count;
        return count;
      }

      // no overflow allowed
      int_type overflow(int_type /*ch*/) override {
        assert(false);
        return 0;
      }
    
  private:
      static const unsigned int length = 1024*1024; // 1MB buffer should be enough for one message
      char buffer[length];
      size_t position;
  };


public:

  DirectLogfileManager()
    : 
    valid_frame(false),
    stream(&buffer)
  {
  }

  ~DirectLogfileManager()
  {
    closeFile();
  }

  std::ostream& log(unsigned int frameNumber, const std::string& name)
  {
    flushFrame();
    valid_frame = true;

    buffer.clear();
    this->name = name;
    this->frameNumber = frameNumber;
    return stream;
  }

  void openFile(const char* filePath)
  {
    closeFile();
    outFile.open(filePath, std::ios::out | std::ios::binary);
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
  * Empty buffer and write to file
  */
  void flush()
  {
    flushFrame();
    outFile.flush();
  }

  size_t getWrittentBytesCount() const {
    return std::max(static_cast<size_t>(0), static_cast<size_t>(outFile.tellp()));
  }

  bool is_ready() const {
    return outFile.is_open() && !outFile.fail();
  }

private:

  void flushFrame() 
  {
    if(!outFile.is_open() || !valid_frame) {
      return;
    }

    // NOTE: this also happens when the memory is full, so we use it only for debug
    // make sure the data stream is alright
    //ASSERT(outFile.good());

    outFile.write((const char*)(&frameNumber), sizeof(unsigned int));
    outFile << name << '\0';

    outFile.write((const char*) &buffer.size(), 4);
    outFile.write(buffer.buf(), buffer.size());

    valid_frame = false;
  }


private:  
  mutable std::ofstream outFile;

  bool valid_frame;
  unsigned int frameNumber;
  std::string name;
  SimpleBuffer buffer;
  std::ostream stream;
};

#endif //_DirectLogfileManager_h_




