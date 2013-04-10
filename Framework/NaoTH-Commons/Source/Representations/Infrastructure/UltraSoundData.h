/* 
 * File:   UltraSoundData.h
 * Author: thomas
 *
 * Created on 7. April 2009, 13:36
 */

#ifndef _ULTRASOUNDDATA_H
#define _ULTRASOUNDDATA_H

#include <ostream>
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{
  class UltraSoundData : public Printable
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

  class UltraSoundSendData: public UltraSoundData
  {
  public:
    UltraSoundSendData();
    virtual ~UltraSoundSendData();

    void setMode(unsigned int mode);

    /*
    Simple modes:
    ALDEBARAN documentation:
    - The first bit of this value lets you choose which receiver you want to use. 0 for left and 1 for right.
    - The second bit lets you choose the transmitter. Again, 0 meaning left and 1 meaning right.
    */
    static const int LEFT_LEFT = 0;
    static const int LEFT_RIGHT = 1;
    static const int RIGHT_LEFT = 2;
    static const int RIGHT_RIGHT = 3;

    /*
    Extended modes:
    ALDEBARAN documentation:
    - The third bit (value 4) means that two captures will be made with only one command, left and right. 
        The results will be available in two new subDevices: US/Left/Sensor and US/Right/Sensor. 
        Now, 10 values are available for each sensor, corresponding to the distance of 
        the first 10 echoes detected.
    - The fourth bit (value 8) orders the use of two transmitters at the same time.
    - The seventh bit (value 64) is used to register for a periodic capture. It means that you need to 
        send the read request to the DCM just once, then you simply read the values every 100 ms in ALMemory. 
        To stop the periodic mode, you can send a new reading command without that bit.
    */
    static const int TWO_CAPTURES = 4;
    static const int TWO_TRANSMITTERS = 8;
    static const int PERIODIC = 64;


    virtual void print(std::ostream& stream) const;

    unsigned int ultraSoundTimeStep;
    unsigned int mode;
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

