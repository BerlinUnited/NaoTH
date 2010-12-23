// :mode=c++:
/*
encode.h - c++ wrapper for a base64 encoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64

 Modified for SimSpark (http://simspark.sourceforge.net)
 */
#ifndef BASE64_ENCODE_H
#define BASE64_ENCODE_H

#include <iostream>
#include <sstream>

namespace base64 {
extern "C" {
#include "cencode.h"
}

class Encoder
{
public:

  Encoder(int buffersize_in = 128)
  : _buffersize(buffersize_in)
  {
    _code = new char[2 * _buffersize];
  }

  ~Encoder()
  {
    delete [] _code;
  }

  void encode(std::istream& istream_in, std::ostream& ostream_in)
  {
    // get length
    istream_in.seekg(0, std::ios::end);
    int length = istream_in.tellg();
    istream_in.seekg(0, std::ios::beg);

    // allocate memory:
    char* buffer = new char [length];

    // read data as a block:
    istream_in.read(buffer, length);

    std::string code = encode(buffer, length);
    ostream_in << code;
  }

  std::string encode(const char* in, int len)
  {
    base64_init_encodestate(&_state);

    int plainlength;
    int codelength;
    std::stringstream ss;

    while (len > 0) {
      if (len > _buffersize) {
        plainlength = _buffersize;
      } else {
        plainlength = len;
      }
      //
      codelength = encode(in, plainlength, _code);
      
      ss.write(_code, codelength);

      in += plainlength;
      len -= plainlength;
    }

    codelength = encode_end(_code);
    ss.write(_code, codelength);

    return ss.str();
  }

protected:

  int encode(char value_in)
  {
    return base64_encode_value(value_in);
  }

  int encode(const char* code_in, const int length_in, char* plaintext_out)
  {
    return base64_encode_block(code_in, length_in, plaintext_out, &_state);
  }

  int encode_end(char* plaintext_out)
  {
    return base64_encode_blockend(plaintext_out, &_state);
  }

private:
  base64_encodestate _state;
  int _buffersize;
  char* _code;
};

} // namespace base64

#endif // BASE64_ENCODE_H

