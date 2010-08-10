/* 
 * File:   Motion.cpp
 * Author: thomas
 * 
 * Created on 10. August 2010, 17:27
 */

#include "Motion.h"

#include <iostream>

#include <PlatformInterface/Platform.h>

Motion::Motion()
{
  Platform::getInstance().thePlatformInterface->registerMotionCallback(this);
}

void Motion::call()
{
  std::cout << "Motion was called" << std::endl;
}

Motion::~Motion()
{
}

