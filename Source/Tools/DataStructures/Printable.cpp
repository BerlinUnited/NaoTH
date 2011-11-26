#include "Printable.h"

namespace naoth
{
  ostream& operator<<(ostream& stream, const Printable& printable)
  {
    printable.print(stream);
    return stream;
  }
}
