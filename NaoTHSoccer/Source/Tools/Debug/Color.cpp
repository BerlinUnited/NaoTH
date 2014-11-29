
#include "Color.h"
#include "Tools/Math/Common.h"

std::ostream& operator<<(std::ostream& stream, const Color& color)
{
  stream << color.toString();
  return stream;
}


Color::Color()
{
  (*this)[red]   = 0.0;
  (*this)[green] = 0.0;
  (*this)[blue]  = 0.0;
  (*this)[alpha] = 1.0;
}

Color::Color(const Vector_n<double,4>& colorVector)
  : Vector_n<double,4>(colorVector)
{
}

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  (*this)[red]   = static_cast<double>(r)/255.0;
  (*this)[green] = static_cast<double>(g)/255.0;
  (*this)[blue]  = static_cast<double>(b)/255.0;
  (*this)[alpha] = static_cast<double>(a)/255.0;
}

Color::Color(double r, double g, double b, double a)
{
  (*this)[red]   = r;
  (*this)[green] = g;
  (*this)[blue]  = b;
  (*this)[alpha] = a;
}

Color::Color(const char* color)
{
  (*this)[red]   = static_cast<double>(16*hexCharToInt(color[0]) + hexCharToInt(color[1]))/255.0;
  (*this)[green] = static_cast<double>(16*hexCharToInt(color[2]) + hexCharToInt(color[3]))/255.0;
  (*this)[blue]  = static_cast<double>(16*hexCharToInt(color[4]) + hexCharToInt(color[5]))/255.0;
  if(strlen(color) == 8)
    (*this)[alpha] = static_cast<double>(16*hexCharToInt(color[6]) + hexCharToInt(color[7]))/255.0;
}

Color::Color(unsigned int colorIndex)
{
  unsigned int r = (((colorIndex)*224) % 255);
  unsigned int g = (((colorIndex + 197)*1034345) % 255);
  unsigned int b = (((colorIndex + 23)*74353) % 255);

  //  too dark or too bright?
  if(((r+b+g) / 3) < 100 )
  {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }
  else if(((r+b+g) / 3) > 192 )
  {
    r = 1*(r / 2);
    g = 1*(g / 2);
    b = 1*(b / 2);
  }

  if(r == 255 && g == 255 && b == 255)
  {
    r = 255;
    g = 255;
    b = 0;
  }

  (*this)[red] = ((double) r) / 255.0;
  (*this)[green] = ((double) g) / 255.0;
  (*this)[blue] = ((double) b) / 255.0;
  (*this)[alpha] = 1.0;
}

unsigned char Color::getRed() const
{ 
  return static_cast<unsigned char>(Math::clamp(((*this)[red]*255.0+0.5),0.0,255.0)); 
}
unsigned char Color::getGreen() const
{ 
  return static_cast<unsigned char>(Math::clamp(((*this)[green]*255.0+0.5),0.0,255.0)); 
}
unsigned char Color::getBlue() const
{ 
  return static_cast<unsigned char>(Math::clamp(((*this)[blue]*255.0+0.5),0.0,255.0)); 
}
unsigned char Color::getAlpha() const
{ 
  return static_cast<unsigned char>(Math::clamp(((*this)[alpha]*255.0+0.5),0.0,255.0)); 
}

std::string Color::toString() const
{    
  std::string result;
  result.append(charToHexString(getRed()));
  result.append(charToHexString(getGreen()));
  result.append(charToHexString(getBlue()));
  result.append(charToHexString(getAlpha()));
  return result;
}//end toString

std::string Color::charToHexString(unsigned char c)
{
  int n = c;
  int second = n%16;
  int first = n/16;

  std::stringstream ss;
  ss << intToHexChar(first) << intToHexChar(second);
  return ss.str();
}//end charToHexString

int Color::hexCharToInt(char c)
{
  switch(c)
  {
  case '0': return 0;
  case '1': return 1;
  case '2': return 2;
  case '3': return 3;
  case '4': return 4;
  case '5': return 5;
  case '6': return 6;
  case '7': return 7;
  case '8': return 8;
  case '9': return 9;
  case 'A': return 10;
  case 'B': return 11;
  case 'C': return 12;
  case 'D': return 13;
  case 'E': return 14;
  case 'F': return 15;
  default: return -1;
  }//end switch
}//end intToHexChar

char Color::intToHexChar(int n)
{
  switch(n)
  {
  case 0: return '0';
  case 1: return '1';
  case 2: return '2';
  case 3: return '3';
  case 4: return '4';
  case 5: return '5';
  case 6: return '6';
  case 7: return '7';
  case 8: return '8';
  case 9: return '9';
  case 10: return 'A';
  case 11: return 'B';
  case 12: return 'C';
  case 13: return 'D';
  case 14: return 'E';
  case 15: return 'F';
  default: return 'X';
  }//end switch

  return 'X';
}//end intToHexChar
