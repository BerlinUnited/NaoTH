/* 
 * File:   UltraSoundData.h
 * Author: thomas
 *
 * Created on 7. April 2009, 13:36
 */

#ifndef _ULTRASOUNDDATA_H
#define	_ULTRASOUNDDATA_H

#include <ostream>
#include "Interface/PlatformInterface/PlatformInterchangeable.h"
#include "Interface/Tools/DataStructures/Printable.h"

namespace naorunner
{
  class UltraSoundData : public PlatformInterchangeable, public Printable
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

  class UltraSoundReceiveData: public UltraSoundData
  {
  public:
     UltraSoundReceiveData();
    virtual ~UltraSoundReceiveData();

    virtual void print(std::ostream& stream) const;
    void init();

    double dataLeft[numOfIRSend];
    double dataRight[numOfIRSend];
  };

  class UltraSoundSendData: public UltraSoundData
  {
  public:
     UltraSoundSendData();
    virtual ~UltraSoundSendData();

    void setMode(unsigned int mode, UltraSoundReceiveData& receiver);

    virtual void print(std::ostream& stream) const;
  };
}

#endif	/* _ULTRASOUNDDATA_H */

