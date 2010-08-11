#include "naorunner/Representations/Infrastructure/IRData.h"

using namespace naorunner;

IRSendData::IRSendData() : changed(false)
{

}

IRSendData::~IRSendData()
{

}

string IRSendData::getIRSendName(IRSendID id)
{
  switch(id)
  {
    case Beacon: return "Beacon"; break;
    case Byte1: return "Byte1"; break;
    case Byte2: return "Byte2"; break;
    case Byte3: return "Byte3"; break;
    case Byte4: return "Byte4"; break;
    case RCByte1: return "RCByte1"; break;
    case RCByte2: return "RCByte2"; break;
    default:
      return "Unknown IRSendID";
  }//end switch
}//end getIRSendName

IRReceiveData::IRReceiveData()
{

}

IRReceiveData::~IRReceiveData()
{
  
}
  
string IRReceiveData::getIRReceiveName(IRReceiveID id)
{
  switch(id)
  {
    case RightBeacon: return "RightBeacon"; break;
    case RightByte1: return "RightByte1"; break;
    case RightByte2: return "RightByte2"; break;
    case RightByte3: return "RightByte3"; break;
    case RightByte4: return "RightByte4"; break;
    case RightRCByte1: return "RightRCByte1"; break;
    case RightRCByte2: return "RightRCByte2"; break;
    case LeftBeacon: return "LeftBeacon"; break;
    case LeftByte1: return "LeftByte1"; break;
    case LeftByte2: return "LeftByte2"; break;
    case LeftByte3: return "LeftByte3"; break;
    case LeftByte4: return "LeftByte4"; break;
    case LeftRCByte1: return "LeftRCByte1"; break;
    case LeftRCByte2: return "LeftRCByte2"; break;
    default:
      return "Unknown IRReceiveID";
  }//end switch
}//end getIRReceiveName


void IRReceiveData::print(ostream& stream) const
{
  for(int i = 0; i < numOfIRReceive; i++)
  {
    stream << getIRReceiveName((IRReceiveID)i) << " = " << data[i] << endl;
  }//end for
}//end print
