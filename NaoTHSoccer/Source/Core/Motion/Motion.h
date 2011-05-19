/**
 * @file Motion.h
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#ifndef MOTION_H
#define	MOTION_H

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>
#include "MotionBlackBoard.h"
#include "AbstractMotion.h"

class Motion : public naoth::Callable
{
public:
  Motion();
  virtual ~Motion();

  virtual void call();

  void init(naoth::PlatformDataInterface& platformInterface);
  
protected:
  void sensorUpdate();
  
  void motionExecute();
  
  void processSensorData();
  
  void postProcess();
  
private:
  MotionBlackBoard& theBlackBoard;

  EmptyMotion theEmptyMotion;
};

#endif	/* MOTION_H */

