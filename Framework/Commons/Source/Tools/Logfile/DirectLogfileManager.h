/** 
 * logging manager 
 * @author: Heinrich
 */

#ifndef _DirectLogfileManager_h_
#define _DirectLogfileManager_h_

#include <cstdint>
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
    stream(&buffer),
    writtenBytes(0)
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
    // NOTE: 'unsigned int' might be larger on some platforms, but here we NEED 4 bytes
    this->frameNumber = static_cast<uint32_t>(frameNumber);
    return stream;
  }

  void openFile(const std::string& filePath)
  {
    closeFile();
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
  * Empty buffer and write to file
  */
  void flush()
  {
    flushFrame();
    outFile.flush();
  }

  size_t getWrittentBytesCount() const {
    return writtenBytes;
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

    // NOTE: sizeof(frameNumber) has to be 4 bytes
    outFile.write((const char*)(&frameNumber), sizeof(frameNumber));
    writtenBytes += 4;

    outFile << name << '\0';
    writtenBytes += name.size() + 1;

    outFile.write((const char*) &buffer.size(), 4);
    writtenBytes += 4;

    outFile.write(buffer.buf(), buffer.size());
    writtenBytes += buffer.size();

    valid_frame = false;
  }


private:  
  mutable std::ofstream outFile;

  bool valid_frame;
  uint32_t frameNumber; // NOTE: the logfile format expects 4 byte
  std::string name;
  SimpleBuffer buffer;
  std::ostream stream;
  size_t writtenBytes;
};

#endif //_DirectLogfileManager_h_




