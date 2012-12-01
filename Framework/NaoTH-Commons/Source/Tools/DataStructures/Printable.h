/* 
 * File:   Printable.h
 * Author: thomas
 *
 * Created on 12. Februar 2010, 14:51
 */

#ifndef _Printable_H_
#define _Printable_H_

#include <ostream>

namespace naoth
{

  class Printable
  {
  public:
    /**
     * This method must be overwritten bei a particular
     * Printable in order to stream out some specific (visible) data
     */
    virtual void print(std::ostream& stream) const = 0;

<<<<<<< HEAD
    friend ostream& operator<<(ostream& stream, const Printable& printable);
=======
    virtual ~Printable()
    {
    }

    friend std::ostream& operator<<(std::ostream& stream, const Printable& printable);

>>>>>>> master
  };
}

#endif  /* _Printable_H_ */

