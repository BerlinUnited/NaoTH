/* 
 * File:   ButtonData.cpp
 * Author: thomas
 * 
 * Created on 6. march 2009, 10:40
 */

#include "ButtonData.h"

#include <string>

using namespace naoth;
using namespace std;

ButtonData::ButtonData() 
{
  for(int i=0; i < numOfButtons; i++)
  {
    isPressed[i] = false;
    numOfFramesPressed[i] = 0;
    eventCounter[i] = 0;
  }
}

ButtonData::~ButtonData()
{
}

string ButtonData::getButtonName(ButtonID id)
{
  switch(id)
  {
    case Chest: return "Chest";
    case LeftFootLeft: return "LeftFootLeft";
    case LeftFootRight: return "LeftFootRight";
    case RightFootLeft: return "RightFootLeft";
    case RightFootRight: return "RightFootRight";

    case HeadFront: return "HeadFront";
    case HeadMiddle: return "HeadMiddle";
    case HeadRear: return "HeadRear";

    case LeftHandBack: return "LeftHandBack";
    case LeftHandLeft: return "LeftHandLeft";
    case LeftHandRight: return "LeftHandRight";

    case RightHandBack: return "RightHandBack";
    case RightHandLeft: return "RightHandLeft";
    case RightHandRight: return "RightHandRight";

    default: return "Unknown Button";
  }//end switch
}//end getBumperName

void ButtonData::print(ostream& stream) const
{
  stream << "Button: Pressed, #Pressed, #Events" << std::endl;
  for(int i = 0; i < numOfButtons; i++)
  {
    stream  << getButtonName((ButtonID)i) << ": " 
            << isPressed[i] << ", "
            << numOfFramesPressed[i] << ", "
            << eventCounter[i]
            << std::endl;
  }
}//end print


