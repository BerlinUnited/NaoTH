/* 
 * File:   UltraSoundData.h
 * Author: thomas
 *
 * Created on 7. April 2009, 13:36
 */

#ifndef _ULTRASOUNDDATA_H
#define	_ULTRASOUNDDATA_H

#include "Tools/ModuleFramework/Representation.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"

class UltraSoundData : public PlatformInterchangeable
{
public:
  enum UltraSoundID
  {
    distance_1,
    distance_2,
    distance_3,
    distance_4,
    distance_5,
    distance_6,
    distance_7,
    distance_8,
    distance_9,
    numOfIRSend
  };
  
  UltraSoundData();
  virtual ~UltraSoundData();

  double data;
  unsigned int ultraSoundTimeStep;
};

class UltraSoundReceiveData: public UltraSoundData, public Printable
{
public:
   UltraSoundReceiveData();
  virtual ~UltraSoundReceiveData();

  virtual void print(ostream& stream) const;
  void init();

  double dataLeft[numOfIRSend];
  double dataRight[numOfIRSend];
};

class UltraSoundSendData: public UltraSoundData, public Printable
{
public:
   UltraSoundSendData();
  virtual ~UltraSoundSendData();

  void setMode(unsigned int mode, UltraSoundReceiveData& receiver);

  virtual void print(ostream& stream) const;
};

REPRESENTATION_INTERFACE(UltraSoundReceiveData);
REPRESENTATION_INTERFACE(UltraSoundSendData);

#endif	/* _ULTRASOUNDDATA_H */

