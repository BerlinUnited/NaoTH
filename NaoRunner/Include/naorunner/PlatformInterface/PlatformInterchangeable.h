/* 
 * File:   PlatformInterchangeable.h
 * Author: thomas
 *
 * Created on 12. Februar 2010, 15:27
 */

#ifndef _PLATFORMINTERCHANGEABLE_H
#define	_PLATFORMINTERCHANGEABLE_H

namespace naorunner
{

  /**
   *  Empty interface marking objects as interchangeable in the PlatformInterface
   */
  class PlatformInterchangeable
  {

  public:

    /** HACK: Dummy for dynamic cast */
    virtual void dummyPlatformInterchangeable()
    {

    };

    virtual ~PlatformInterchangeable()
    {

    }

  };
}
#endif	/* _PLATFORMINTERCHANGEABLE_H */

