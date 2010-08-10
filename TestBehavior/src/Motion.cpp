/* 
 * File:   Motion.cpp
 * Author: thomas
 * 
 * Created on 10. August 2010, 17:27
 */

#include "Motion.h"

#include <iostream>

Motion::Motion()
{
}

void Motion::call()
{
  std::cout << "Motion was called" << std::endl;
}

Motion::~Motion()
{
}

