/**
 * @file Motion.h
 *
  * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef MOTION_H
#define	MOTION_H

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>

class Motion : public naoth::Callable
{
public:
  Motion();
  virtual ~Motion();

  template<class PlatformType>
  void init(naoth::PlatformInterface<PlatformType>& platformInterface)
  {
    std::cout << "Motion register start" << std::endl;

    std::cout << "Motion register end" << std::endl;
  }//end init

  virtual void call();

};

#endif	/* MOTION_H */

