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


// register all modules here
extern "C" G_MODULE_EXPORT void naoth_register_modules(ModuleManager* manager)
{
  manager->registerModule<PackageTest> ("PackageTest"); 
}


