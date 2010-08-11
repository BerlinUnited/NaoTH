/* 
 * File:   IRData.h
 * Author: Oliver Welter
 *
 * Created on 11. März 2009, 11:03
 */

#ifndef _IRDATA_H
#define	_IRDATA_H

#include <string>

#include "naorunner/PlatformInterface/PlatformInterchangeable.h"
#include "naorunner/Tools/DataStructures/Printable.h"

using namespace std;

class IRSendData : public PlatformInterchangeable
{
private:
public:
  bool changed;
  enum IRSendID
  {
    Beacon,
    Byte1,
    Byte2,
    Byte3,
    Byte4,
    RCByte1,
    RCByte2,
    numOfIRSend
  };

  int data[numOfIRSend];

  IRSendData();
  ~IRSendData();

  static string getIRSendName(IRSendID id);
};

class IRReceiveData : public PlatformInterchangeable, public Printable
{
private:
public:
  enum IRReceiveID
  {
    RightBeacon,
    RightByte1,
    RightByte2,
    RightByte3,
    RightByte4,
    RightRCByte1,
    RightRCByte2,
    LeftBeacon,
    LeftByte1,
    LeftByte2,
    LeftByte3,
    LeftByte4,
    LeftRCByte1,
    LeftRCByte2,
    numOfIRReceive
  };

  int data[numOfIRReceive];
  IRReceiveData();
  ~IRReceiveData();

  static string getIRReceiveName(IRReceiveID id);

  virtual void print(ostream& stream) const;
};

#endif	/* _IRDATA_H */

