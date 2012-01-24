/*
 * File:   Test.cpp
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */

#include "PackageTest.h"

#include <gmodule.h>

extern "C++"
{
  #include <ModuleFramework/ModuleManager.h>
}

PackageTest::PackageTest()
{

}

void PackageTest::execute()
{
  std::cout << "Hello Test, time is:" << std::endl << getFrameInfo().getTime() << std::endl;

  getColorTable64().setColorClass(ColorClasses::blue, 100, 100, 100);
  
  Pixel p;
  p.y = 100;
  p.u = 100;
  p.v = 100;
  std::cout << "new color table value:" << std::endl << getColorTable64().getColorClass(p) << std::endl;

}


PackageTest::~PackageTest()
{
}



