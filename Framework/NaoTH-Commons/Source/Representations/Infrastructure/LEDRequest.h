/* 
 * File:   LEDRequest.h
 * Author: thomas
 *
 * Created on 22. April 2009, 16:44
 */

#ifndef _LEDREQUEST_H
#define	_LEDREQUEST_H

#include "Representations/Infrastructure/LEDData.h"
#include "Tools/DataStructures/Printable.h"

class LEDRequest : public naoth::Printable
{

public:

  LEDRequest()
  {

  }

  virtual void print(ostream& stream) const
  {
    request.print(stream);
  }

  virtual ~LEDRequest()
  {
    
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


#endif	/* _LEDREQUEST_H */

