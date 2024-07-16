/* 
 * File:   LEDRequest.h
 * Author: thomas
 *
 * Created on 22. April 2009, 16:44
 */

#ifndef LED_REQUEST_H
#define LED_REQUEST_H

#include "Representations/Infrastructure/LEDData.h"
#include "Tools/DataStructures/Printable.h"

class LEDRequest : public naoth::Printable
{

public:
  LEDRequest() {}
  virtual ~LEDRequest(){}

  virtual void print(std::ostream& stream) const {
    request.print(stream);
  }

  naoth::LEDData request;
};

class BehaviorLEDRequest : public LEDRequest
{
public:
  virtual ~BehaviorLEDRequest() {}
};

class GameControllerLEDRequest : public LEDRequest
{
public:
  virtual ~GameControllerLEDRequest() {}
};

class WifiModeSetterLEDRequest : public LEDRequest
{
public:
  WifiModeSetterLEDRequest() : ignore(true) {}
  virtual ~WifiModeSetterLEDRequest() {}
  bool ignore;
};

class FrameRateCheckLEDRequest : public LEDRequest
{
public:
  FrameRateCheckLEDRequest() : ignore(true) {}
  virtual ~FrameRateCheckLEDRequest() {}
  bool ignore;
};

class DebugLEDRequest : public LEDRequest
{
public:
  DebugLEDRequest() : enabled(false) {}
  virtual ~DebugLEDRequest() {}
  bool enabled;
};

#endif  /* LED_REQUEST_H */

