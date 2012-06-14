/* 
 * File:   Printable.h
 * Author: thomas
 *
 * Created on 12. Februar 2010, 14:51
 */

#ifndef _PRINTABLE_H
#define _PRINTABLE_H

#include <ostream>

//TODO: remove it
using namespace std;

namespace naoth
{

  class Printable
  {
  public:
    /**
     * This method must be overwritten bei a particular
     * Printable in order to stream out some specific (visible) data
     */
    virtual void print(ostream& stream) const = 0;

    friend ostream& operator<<(ostream& stream, const Printable& printable);
  };
}

#endif  /* _PRINTABLE_H */

