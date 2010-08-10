/* 
 * File:   Motion.h
 * Author: thomas
 *
 * Created on 10. August 2010, 17:27
 */

#ifndef MOTION_H
#define	MOTION_H

#include <PlatformInterface/Callable.h>

class Motion
{
public:
  Motion();
  virtual ~Motion();

  virtual void call();

private:

};

#endif	/* MOTION_H */

