/* 
 * File:   Cognition.h
 * Author: thomas
 *
 * Created on 10. August 2010, 17:27
 */

#ifndef COGNITION_H
#define	COGNITION_H

#include <iostream>
#include <Interface/PlatformInterface/Callable.h>

#include <Interface/Representations/Infrastructure/JointData.h>
#include <Interface/Representations/Infrastructure/LEDData.h>

class Cognition : public naorunner::Callable
{
public:
  Cognition();
  virtual ~Cognition();


  template<class PlatformType>
  void init(PlatformType& platformInterface)
  {
    std::cout << "Cognition register start" << std::endl;
    platformInterface.registerCognitionInput(theSensorJointData, "SensorJointData");

    platformInterface.registerCognitionOutput(theLEDData, "LEDData"); 
    std::cout << "Cognition register end" << std::endl;
  }


  virtual void call();

private:
  naorunner::SensorJointData theSensorJointData;
  naorunner::LEDData theLEDData;
};

#endif	/* COGNITION_H */

