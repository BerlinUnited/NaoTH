#include "naorunner/Tools/DataStructures/Printable.h"

using namespace naorunner;

ostream& operator<<(ostream& stream, const Printable& printable)
{
  printable.print(stream);
  return stream;
}
