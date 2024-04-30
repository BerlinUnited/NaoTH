/* 
 * File:   LEDData.h
 * Author: Martin Martius
 *
 * Created on 24. Februar 2009, 11:25
 */

#ifndef LEDDATA_H
#define LEDDATA_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Common.h"

namespace naoth
{

  class LEDData : public Printable
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

      HeadFrontLeft0,
      HeadFrontLeft1,
      HeadFrontRight0,
      HeadFrontRight1,
      HeadMiddleLeft0,
      HeadMiddleRight0,
      HeadRearLeft0,
      HeadRearLeft1,
      HeadRearLeft2,
      HeadRearRight0,
      HeadRearRight1,
      HeadRearRight2,

      numOfMonoLED
    };

    enum MultiLEDID
    {
      // right eye
      FaceRight0,
      FaceRight45,
      FaceRight90,
      FaceRight135,
      FaceRight180,
      FaceRight225,
      FaceRight270,
      FaceRight315,

      // left eye
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

    static std::string getLEDName(MonoLEDID theMonoLEDID);
    static std::string getLEDName(MultiLEDID theMultiLEDID);


    virtual void print(std::ostream& stream) const;

    // arrays of all monochromic and multi colored LEDs of the robot
    double theMonoLED[numOfMonoLED];
    double theMultiLED[numOfMultiLED][numOfLEDColor];

    // Indicates that requested LED values changed in this frame.
    // On NAO Versisons <= 5, setting LEDs was associated with a processing delay.
    // To reduce the impact of the delay, the LED values were only processed on changed.
    // NOTE: this might be not relevant on NAO6
    // TODO: chaecking changes if necessary should functionality of the platform.
    bool change;

    LEDData();
    ~LEDData() {}

    void setEyeRight(double r, double g, double b) { 
      setMultiLEDs(LEDData::FaceRight0, LEDData::FaceRight315, r, g, b);
    }

    void setEyeLeft(double r, double g, double b) { 
      setMultiLEDs(LEDData::FaceLeft0, LEDData::FaceLeft315, r, g, b);
    }

    void setFootRight(double r, double g, double b) { 
      setMultiLEDs(LEDData::FootRight, LEDData::FootRight, r, g, b);
    }

    void setFootLeft(double r, double g, double b) { 
      setMultiLEDs(LEDData::FootLeft, LEDData::FootLeft, r, g, b);
    }

    void setChestButton(double r, double g, double b) { 
      setMultiLEDs(LEDData::ChestButton, LEDData::ChestButton, r, g, b);
    }

    // mono LEDs

    void setEarRight(double value) { 
      setMontoLEDs(LEDData::EarRight0, LEDData::EarRight324, value);
    }

    void setEarLeft(double value) { 
      setMontoLEDs(LEDData::EarLeft0, LEDData::EarLeft324, value);
    }

    void setHead(double value) { 
      setMontoLEDs(LEDData::HeadFrontLeft0, LEDData::HeadRearRight2, value);
    }

  private: // internal helper methods

    // set a range of monochromatic LEDs to a specific value
    void setMontoLEDs(int from, int to, double value)
    {
      for(int i=from; i <= to; i++) {
        theMonoLED[i] = Math::clamp(value, 0.0, 1.0);
      }
    }

    // set a range of multi color LEDs to a specific color
    void setMultiLEDs(int from, int to, double r, double g, double b)
    {
      for(int i = from; i <= to; i++)
      {
        theMultiLED[i][RED]   = Math::clamp(r, 0.0, 1.0);
        theMultiLED[i][GREEN] = Math::clamp(g, 0.0, 1.0);
        theMultiLED[i][BLUE]  = Math::clamp(b, 0.0, 1.0);
      }
    }

  };
}

#endif  /* LEDDATA_H */

