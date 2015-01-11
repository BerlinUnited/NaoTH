/**
 * @file ASCIIEncoder.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief encode/decode data into ASCII string, the range is the ASCII printing character
 * subset [0x20, 0x7E] except the white space character and the normal brackets ( and ).
 *
 */

#ifndef _ASCIIENCODER_H
#define _ASCIIENCODER_H

#include <limits>
#include <string>
#include "Tools/Math/Pose2D.h"
#include "Tools/Debug/NaoTHAssert.h"

class ASCIIEncoder
{
private:
  char min_char;
  char max_char;
  char double_quote_char;
  char single_quote_char;
  char left_brace_char;
  char right_brace_char;
  char back_slash_char;
  int encode_base;

public:

  ASCIIEncoder()
  {
    min_char = 0x21;
    max_char = 0x7E;
    double_quote_char = '"';
    single_quote_char = '\'';
    left_brace_char = '(';
    right_brace_char = ')';
    back_slash_char = '\\';
    encode_base = max_char - min_char - 5 + 1;
  }

  std::string encode(unsigned int data, size_t size) const
  {
    ASSERT(size > 0);

    unsigned int n = data % encode_base;
    char c = static_cast<char> (n + min_char);
    if (c >= double_quote_char) c++;
    if (c >= single_quote_char) c++;
    if (c >= left_brace_char) c++;
    if (c >= right_brace_char) c++;
    if (c >= back_slash_char) c++;

    std::string s;
    if (size > 1)
      s = encode(data / encode_base, size - 1);
    return s + c;
  }

  unsigned int decodeUnsigned(const std::string& data) const
  {
    size_t size = data.size();
    ASSERT(size > 0);

    char c = data[size - 1];
    ASSERT(c >= min_char);
    ASSERT(c <= max_char);
    // reverse order compared to encode (!)
    if (c > back_slash_char) c--;
    if (c > right_brace_char) c--;
    if (c > left_brace_char) c--;
    if (c > single_quote_char) c--;
    if (c > double_quote_char) c--;
    
    int n = c - min_char;

    if (size > 1) {
      int m = decodeUnsigned(data.substr(0, size - 1));
      n += m * encode_base;
    }

    return n;
  }

  std::string encode(int data, size_t size) const
  {
    int maxValue = maxSigned(size);
    ASSERT(data >= -maxValue);
    ASSERT(data <= maxValue);
    int v = Math::clamp(data, -maxValue, maxValue);

    v += maxValue; // positive value only
    return encode(static_cast<unsigned int> (v), size);
  }

  int decodeSigned(const std::string& data) const
  {
    int maxValue = maxSigned(data.size());
    int v = decodeUnsigned(data);
    return v - maxValue;
  }

  std::string encode(double data, double maxData, size_t size) const
  {
    ASSERT(data >= -maxData);
    ASSERT(data <= maxData);
    int maxValue = maxSigned(size);
    int intValue = static_cast<int> (maxValue / maxData * data);
    return encode(intValue, size);
  }

  double decodeDouble(const std::string& data, double maxData) const
  {
    int maxValue = maxSigned(data.size());
    int intValue = decodeSigned(data);
    return intValue * maxData / maxValue;
  }

  std::string encode(const Vector2<double>& data, const Vector2<double>& maxData, size_t size) const
  {
    ASSERT(data.x >= -maxData.x);
    ASSERT(data.x <= maxData.x);
    ASSERT(data.y >= -maxData.y);
    ASSERT(data.y <= maxData.y);

    unsigned int maxValue = maxUnsigned(size);
    Vector2<double> ud = data + maxData;
    double cellsize = std::sqrt( maxData.x * maxData.y * 4 / maxValue );
    unsigned int cellnumx = static_cast<unsigned int>( maxData.x * 2 / cellsize );

    unsigned int udx = static_cast<unsigned int>(ud.x /  cellsize);
    unsigned int udy = static_cast<unsigned int>(ud.y /  cellsize);
    unsigned int v = udx + udy * cellnumx;
    return encode(v, size);
  }

  Vector2<double> decodeVector2D(const std::string& data, const Vector2<double>& maxData) const
  {
    unsigned int maxValue = maxUnsigned(data.size());
    double cellsize = std::sqrt( maxData.x * maxData.y * 4 / maxValue );
    unsigned int cellnumx = static_cast<unsigned int>( maxData.x * 2 / cellsize );
    unsigned int v = decodeUnsigned(data);
    unsigned int udy = v / cellnumx;
    unsigned int udx = v - udy * cellnumx;
    
    Vector2<double> ud(udx * cellsize, udy * cellsize);
    return ud - maxData;
  }

  std::string encode(const Pose2D& data, const Vector2<double>& maxPos, double anglePiece, size_t size) const
  {
    ASSERT(anglePiece >= -Math::pi);
    ASSERT(anglePiece <= Math::pi);
    ASSERT(data.rotation >= -Math::pi);
    ASSERT(data.rotation <= Math::pi);
    ASSERT(data.translation.x >= -maxPos.x);
    ASSERT(data.translation.x <= maxPos.x);
    ASSERT(data.translation.y >= -maxPos.y);
    ASSERT(data.translation.y <= maxPos.y);

    unsigned int angleNum = static_cast<unsigned int>(Math::pi2 / anglePiece);
    ASSERT(angleNum > 0);
    unsigned int maxValue = maxUnsigned(size) / angleNum;
    double cellsize = std::sqrt( maxPos.x * maxPos.y * 4 / maxValue );
    unsigned int cellnumx = static_cast<unsigned int>( maxPos.x * 2 / cellsize );

    Vector2<double> ud = data.translation + maxPos;
    unsigned int udx = static_cast<unsigned int>(ud.x /  cellsize);
    unsigned int udy = static_cast<unsigned int>(ud.y /  cellsize);
    unsigned int ua = static_cast<unsigned int>((data.rotation + Math::pi ) / anglePiece);
    unsigned int v = static_cast<unsigned int>(udx + udy * cellnumx + ua * maxValue);
    return encode(v, size);
  }
  
  Pose2D decodePose2D(const std::string& data, const Vector2<double>& maxPos, double anglePiece) const
  {
    ASSERT(anglePiece >= -Math::pi);
    ASSERT(anglePiece <= Math::pi);
    
    unsigned int angleNum = static_cast<unsigned int>(Math::pi2 / anglePiece);
    ASSERT(angleNum > 0);
    unsigned int maxValue = maxUnsigned(data.size()) / angleNum;

    double cellsize = std::sqrt( maxPos.x * maxPos.y * 4 / maxValue );
    unsigned int cellnumx = static_cast<unsigned int>( maxPos.x * 2 / cellsize );
    
    unsigned int v = decodeUnsigned(data);
    unsigned int ua = v / maxValue;
    v -= ua * maxValue;
    unsigned int udy = v / cellnumx;
    unsigned int udx = v - udy * cellnumx;

    Vector2<double> ud(udx * cellsize, udy * cellsize);
    Pose2D result;
    result.translation = ud - maxPos;
    result.rotation = ua * anglePiece - Math::pi;
    return result;
  }

  unsigned int maxUnsigned(size_t size) const
  {
    return static_cast<unsigned int> (std::min(static_cast<double> (std::numeric_limits<unsigned int>::max()),
      std::pow((double) encode_base, (int) size)));
  }

  int maxSigned(size_t size) const
  {
    return static_cast<int> (std::min(static_cast<double> (std::numeric_limits<int>::max()),
      (std::pow((double) encode_base, (int) size) - 1.0) / 2.0));
  }
};

#endif  /* _ASCIIENCODER_H */

