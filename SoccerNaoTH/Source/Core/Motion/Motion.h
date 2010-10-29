/**
 * @file Motion.h
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 *
 */

#ifndef MOTION_H
#define	MOTION_H

#include <iostream>
#include <list>
#include <Representations/Infrastructure/JointData.h>
#include <PlatformInterface/Callable.h>

class Motion : public naoth::Callable
{
public:
  Motion();
  virtual ~Motion();

  template<class PlatformType>
  void init(PlatformType& platformInterface)
  {
    std::cout << "Motion register start" << std::endl;

    std::cout << "Motion register end" << std::endl;
  }//end init

  virtual void call();

};

#endif	/* MOTION_H */

