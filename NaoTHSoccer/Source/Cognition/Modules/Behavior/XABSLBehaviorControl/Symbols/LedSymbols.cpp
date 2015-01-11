/**
 * @file LedSymbols.cpp
 *
 * @author <a href="mailto:martius@informatik.hu-berlin.de">Martin Martius</a>
 * Implementation of class LedSymbols
 */


#include "LedSymbols.h"

using namespace naoth;

void LedSymbols::registerSymbols(xabsl::Engine& engine)
{
  // register the eyeLED enum
  engine.registerEnumElement("eyeLED", "eyeLED.off", 0);
  engine.registerEnumElement("eyeLED", "eyeLED.red", 1);
  engine.registerEnumElement("eyeLED", "eyeLED.green", 2);
  engine.registerEnumElement("eyeLED", "eyeLED.blue", 3);
  engine.registerEnumElement("eyeLED", "eyeLED.red_green", 4);

  engine.registerDecimalOutputSymbol("set_right_ear", &setRightEar, &getRightEar);
  engine.registerDecimalOutputSymbol("set_left_ear", &setLeftEar, &getLeftEar);

  engine.registerDecimalOutputSymbol("set_head", &setHead, &getHead);

  engine.registerEnumeratedOutputSymbol("set_right_eye", "eyeLED", &setRightEye, &getRightEye);
  engine.registerEnumeratedOutputSymbol("set_left_eye", "eyeLED", &setLeftEye, &getLeftEye);

}//end registerSymbols


LedSymbols* LedSymbols::theInstance = NULL;

void LedSymbols::execute()
{
}

void LedSymbols::setHead(double id)
{

  if (id <= 6 && theInstance->lastHead != id) {

    for (int i = LEDData::HeadFrontLeft0; i <= LEDData::HeadRearRight2; i++) {

        theInstance->getBehaviorLEDRequest().request.theMonoLED[i] = 0.0;

    }

    switch((int)id) {
        case 6: {
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadFrontLeft1] = 1.0;
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadFrontRight1] = 1.0;
        }
        case 5: {
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadFrontLeft0] = 1.0;
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadFrontRight0] = 1.0;
        }
        case 4: {
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadMiddleLeft0] = 1.0;
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadMiddleRight0] = 1.0;
        }
        case 3: {
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadRearLeft0] = 1.0;
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadRearRight0] = 1.0;
        }
        case 2: {
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadRearLeft1] = 1.0;
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadRearRight1] = 1.0;
        }
        case 1: {
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadRearLeft2] = 1.0;
            theInstance->getBehaviorLEDRequest().request.theMonoLED[LEDData::HeadRearRight2] = 1.0;
        }
        case 0:
        default:
            break;
    }

    theInstance->lastHead = id;
  }

}//end setHead

double LedSymbols::getHead()
{
  return theInstance->lastHead;
}

void LedSymbols::setRightEar(double id)
{
  id = floor(id + 0.5);
  if (id <= 10.0)
  {
    theInstance->lastREar = id;
    int counter = 0;
    for (int i = LEDData::EarRight0; i <= LEDData::EarRight324; i++)
    {
      if (counter < id) {
        theInstance->getBehaviorLEDRequest().request.theMonoLED[i] = 1.0;
      } else {
        theInstance->getBehaviorLEDRequest().request.theMonoLED[i] = 0.0;
      }
      counter++;
    }
  }

}//end setRightEar

double LedSymbols::getRightEar()
{
  return theInstance->lastREar;
}

void LedSymbols::setLeftEar(double id)
{

  id = floor(id + 0.5);
  if (id <= 10.0)
  {
    theInstance->lastLEar = id;
    int counter = 0;
    for (int i = LEDData::EarLeft0; i <= LEDData::EarLeft324; i++)
    {
      if (counter < id) {
        theInstance->getBehaviorLEDRequest().request.theMonoLED[i] = 1.0;
      } else {
        theInstance->getBehaviorLEDRequest().request.theMonoLED[i] = 0.0;
      }
      counter++;
    }
  }

}//end setLeftEar

double LedSymbols::getLeftEar()
{
  return theInstance->lastLEar;
}

void LedSymbols::setEye(LEDData::MultiLEDID from, LEDData::MultiLEDID to, int value)
{
  for (int i = from; i <= to; i++)
  {
    if(value == 0)
    {
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE] = 0.0;
    }
    if (value == 1) // red
    {
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED] = 1.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE] = 0.0;
    }
    else if (value == 2) // green
    {
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN] = 1.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE] = 0.0;
    }
    else if (value == 3) // blue
    {
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN] = 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE] = 1.0;
    }
    else if (value == 4) // red_green
    {
      double red = 1.0;
      double green = 1.0;
      double blue = 0.0;

      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::RED]
        = (i % 2 == 0) ? red : 0.0;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::GREEN]
        = (i % 2 == 0) ? 0.0 : green;
      getBehaviorLEDRequest().request.theMultiLED[i][LEDData::BLUE]
        = (i % 2 == 0) ? blue : 0.0;
    }
  }
}

void LedSymbols::setLeftEye(int value)
{
  theInstance->lastREye = value;
  theInstance->setEye(LEDData::FaceLeft0, LEDData::FaceLeft315, value);
}

int LedSymbols::getRightEye()
{
  return theInstance->lastREye;
}

void LedSymbols::setRightEye(int value)
{
  theInstance->lastLEye = value;
  theInstance->setEye(LEDData::FaceRight0, LEDData::FaceRight315, value);
}

int LedSymbols::getLeftEye()
{
  return theInstance->lastLEye;
}

