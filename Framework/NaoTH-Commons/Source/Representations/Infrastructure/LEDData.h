/* 
 * File:   LEDData.h
 * Author: Martin Martius
 *
 * Created on 24. Februar 2009, 11:25
 */

#ifndef _LEDDATA_H
#define  _LEDDATA_H

#include "Tools/DataStructures/Printable.h"
#include "PlatformInterface/PlatformInterchangeable.h"

namespace naoth
{

  class LEDData : public PlatformInterchangeable, public Printable
  {
  private:
  public:
    enum MonoLEDID
    {
      EarRight0,
      EarRight36,
      EarRight72,
      EarRight108,
      EarRight144,
      EarRight180,
      EarRight216,
      EarRight252,
      EarRight288,
      EarRight324,
      EarLeft0,
      EarLeft36,
      EarLeft72,
      EarLeft108,
      EarLeft144,
      EarLeft180,
      EarLeft216,
      EarLeft252,
      EarLeft288,
      EarLeft324,
      numOfMonoLED
    };

    enum MultiLEDID
    {
      FaceRight0,
      FaceRight45,
      FaceRight90,
      FaceRight135,
      FaceRight180,
      FaceRight225,
      FaceRight270,
      FaceRight315,
      FaceLeft0,
      FaceLeft45,
      FaceLeft90,
      FaceLeft135,
      FaceLeft180,
      FaceLeft225,
      FaceLeft270,
      FaceLeft315,
      FootLeft,
      FootRight,
      ChestButton,
      numOfMultiLED
    };

    enum LEDColor
    {
      RED,
      GREEN,
      BLUE,
      numOfLEDColor
    };


    virtual void print(std::ostream& stream) const;

    double theMonoLED[numOfMonoLED];
    double theMultiLED[numOfMultiLED][numOfLEDColor];
    bool change;
    LEDData();
    ~LEDData();

    static std::string getLEDName(MonoLEDID theMonoLEDID);
    static std::string getLEDName(MultiLEDID theMultiLEDID);

  };
}

#endif  /* _LEDDATA_H */

