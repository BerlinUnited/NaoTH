#include "Printable.h"

namespace naoth
{
  std::ostream& operator<<(std::ostream& stream, const Printable& printable)
  {
    printable.print(stream);
    return stream;
  }
}
