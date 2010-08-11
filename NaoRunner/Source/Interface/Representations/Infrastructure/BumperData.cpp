/* 
 * File:   bumperdata.cpp
 * Author: Oliver Welter
 * 
 * Created on 15. März 2009, 00:11
 */

#include "naorunner/Representations/Infrastructure/BumperData.h"

BumperData::BumperData() {
}

BumperData::~BumperData() {
}

string BumperData::getBumperName(BumperID id)
{
  switch(id)
  {
    case LeftBumperLeft: return("LeftBumperLeft"); break;
    case LeftBumperRight: return("LeftBumperRight"); break;
    case RightBumperLeft: return("RightBumperLeft"); break;
    case RightBumperRight: return("RightBumperRight"); break;
    default: return("Unknown Bumper"); break;
  }//end switch
}//end getBumperName


void BumperData::print(ostream& stream) const
{
  for(int i = 0; i < numOfBumper; i++)
  {
    stream << getBumperName((BumperID)i) << " = " << data[i] << endl;
  }//end for
}//end print
