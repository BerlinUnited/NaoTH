/**
 * @file Cognition.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of the class Cognition
 */

#include "Cognition.h"

/////////////////////////////////////
// Modules
/////////////////////////////////////
//Infrastructure
#include "Modules/Infrastructure/LEDSetter/LEDSetter.h"

using namespace std;

Cognition::Cognition()
{

  registerModule<LEDSetter>("LEDSetter");
}

void Cognition::call()
{
  cout << "Cognition run" << endl;
}//end call

Cognition::~Cognition()
{
}

