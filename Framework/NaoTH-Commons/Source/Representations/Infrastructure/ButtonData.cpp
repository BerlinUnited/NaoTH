/* 
 * File:   ButtonData.cpp
 * Author: thomas
 * 
 * Created on 6. März 2009, 10:40
 */

#include "Representations/Infrastructure/ButtonData.h"

#include <string>

using namespace naoth;

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
    case Chest: return ("Chest"); break;
    case LeftFootLeft: return("LeftFootLeft"); break;
    case LeftFootRight: return("LeftFootRight"); break;
    case RightFootLeft: return("RightFootLeft"); break;
    case RightFootRight: return("RightFootRight"); break;
    default: return("Unknown Button"); break;
  }//end switch
}//end getBumperName

void ButtonData::print(ostream& stream) const
{
  for(int i = 0; i < numOfButtons; i++)
  {
    stream << getButtonName((ButtonID)i) << " = " << isPressed[i] <<endl;
  }//end for
}//end print


