/* 
 * File:   main.cpp
 * Author: thomas
 *
 * Created on 10. August 2010, 18:02
 */


#include <Webots/WebotsController.h>
#include <SimSpark/SimSparkController.h>
#include "Cognition.h"
#include "Motion.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
  //naorunner::WebotsController theController;
  SimSparkController theController;
  theController.init("NaoTH", 3, "localhost", 4100);
  //theController.init();


  Cognition theCognition;
  Motion theMotion;

  theController.registerCallbacks(&theMotion, &theCognition);



  theController.main();

  return 0;
}//end main

