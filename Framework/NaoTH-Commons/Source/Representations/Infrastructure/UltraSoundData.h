/* 
 * File:   UltraSoundData.h
 * Author: thomas
 *
 * Created on 7. April 2009, 13:36
 */

#ifndef _ULTRASOUNDDATA_H
#define  _ULTRASOUNDDATA_H

#include <ostream>
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{
  class UltraSoundData : public PlatformInterchangeable, public Printable
  {
  public:
    enum UltraSoundEchoID
    {
      echo_0,
      echo_1,
      echo_2,
      echo_3,
      echo_4,
      echo_5,
      echo_6,
      echo_7,
      echo_8,
      echo_9,
      numOfUSEcho
    };

    UltraSoundData();
    virtual ~UltraSoundData();

    unsigned int ultraSoundTimeStep;
  };

  class UltraSoundReceiveData: public UltraSoundData
  {
  public:
    UltraSoundReceiveData();
    virtual ~UltraSoundReceiveData();

    virtual void print(std::ostream& stream) const;
    void init();

    double dataLeft[numOfUSEcho];
    double dataRight[numOfUSEcho];
    double rawdata;
  };


  template<>
  class Serializer<UltraSoundReceiveData>
  {
  public:
    static void serialize(const UltraSoundReceiveData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, UltraSoundReceiveData& representation);
  };
}

#endif  /* _ULTRASOUNDDATA_H */

