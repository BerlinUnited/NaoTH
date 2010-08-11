#include "naorunner/Tools/DataStructures/Printable.h"


ostream& operator<<(ostream& stream, const Printable& printable)
{
  printable.print(stream);
  return stream;
}
