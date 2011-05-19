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
#include "MotionBlackBoard.h"

class Motion : public naoth::Callable
{
public:
  Motion();
  virtual ~Motion();

  virtual void call();

  void init(naoth::PlatformDataInterface& platformInterface);

private:
  MotionBlackBoard theBlackBoard;
};

#endif	/* MOTION_H */

