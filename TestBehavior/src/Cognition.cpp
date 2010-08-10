/* 
 * File:   Cognition.cpp
 * Author: thomas
 * 
 * Created on 10. August 2010, 17:27
 */

#include "Cognition.h"

#include <iostream>

Cognition::Cognition()
{
}

void Cognition::call()
{
  std::cout << "Cognition was called" << std::endl;
}

Cognition::~Cognition()
{
}

