/* 
 * File:   main.cpp
 * Author: thomas
 *
 * Created on 10. August 2010, 18:02
 */


#include <WebotsController.h>
#include "Cognition.h"
#include "Motion.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
  WebotsController theWebotsController;
  Cognition theCognition;
  Motion theMotion;

  theWebotsController.registerCallbacks(&theMotion, &theCognition);

  theWebotsController.init();

  theWebotsController.main();

  return 0;
}//end main

