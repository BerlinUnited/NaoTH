/* 
 * File:   Cognition.h
 * Author: thomas
 *
 * Created on 10. August 2010, 17:27
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <Interface/PlatformInterface/Callable.h>

#include <Interface/Representations/Infrastructure/JointData.h>
#include <Interface/Representations/Infrastructure/LEDData.h>

class Cognition : public naorunner::Callable
{
public:
  Cognition();
  virtual ~Cognition();

  virtual void init(naorunner::PlatformInterface& platformInterface);
  virtual void call();

private:
  naorunner::SensorJointData theSensorJointData;
  naorunner::LEDData theLEDData;
};

#endif	/* COGNITION_H */

