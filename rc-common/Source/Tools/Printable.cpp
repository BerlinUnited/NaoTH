#include "Printable.h"

namespace crf
{

ostream& operator<<(ostream& stream, const Printable& printable)
{
  printable.print(stream);
  return stream;
}

}//crf
