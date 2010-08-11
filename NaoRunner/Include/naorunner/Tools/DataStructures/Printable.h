/* 
 * File:   Printable.h
 * Author: thomas
 *
 * Created on 12. Februar 2010, 14:51
 */

#ifndef _PRINTABLE_H
#define	_PRINTABLE_H

#include <ostream>

using namespace std;

class Printable
{
public:

  Printable()
  {}

  /**
   * This method must be overwritten bei a particular
   * Printable in order to stream out some specific (visible) data
   */
  virtual void print(ostream& stream) const = 0;

  virtual ~Printable()
  {
  }

  friend ostream& operator<<(ostream& stream, const Printable& representation);

};

#endif	/* _PRINTABLE_H */

