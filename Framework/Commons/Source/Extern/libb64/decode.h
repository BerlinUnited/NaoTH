// :mode=c++:
/*
decode.h - c++ wrapper for a base64 decoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64

Modified for SimSpark (http://simspark.sourceforge.net)
 */
#ifndef BASE64_DECODE_H
#define BASE64_DECODE_H

#include <iostream>
#include <string>

namespace base64 {
extern "C" {
#include "cdecode.h"
}

class Decoder
{
public:

  void decode(std::istream& istream_in, std::ostream& ostream_in)
  {
    std::string code;
    istream_in >> code;

    int len =static_cast<int> ( code.size());
    char* text = new char[len]; // enough space

    int textLen = decode(code, text, len);

    ostream_in.write((const char*) text, textLen);
  }

  /* Please make sure the out has enough memory (but not bigger than length of in )
   * @return the lenght of decode result
   */
  int decode(const std::string& in, char* out, int out_len)
  {
    base64_init_decodestate(&_state);

    int codelength;
    int plainlength;
    int totalLen = 0;

    const char* code = in.c_str();
    int len = static_cast<int> (in.size());

    while ( len>0 && out_len>0 )
    {
      if (len > out_len)
      {
        codelength = out_len;
      }
      else
      {
        codelength = len;
      }

      plainlength = base64_decode_block(code, codelength, out+totalLen, &_state);

      code += codelength;
      len -= codelength;

      out_len -= plainlength;
      totalLen += plainlength;
    }

    return totalLen;
  }

  int decode(const char* code_in, const int length_in, char* plaintext_out)
  {
    base64_init_decodestate(&_state);
    return base64_decode_block(code_in, length_in, plaintext_out, &_state);
  }

protected:
  int decode(char value_in)
  {
    return base64_decode_value(value_in);
  }

private:
  base64_decodestate _state;
  int _buffersize;
};


} // namespace base64



#endif // BASE64_DECODE_H

