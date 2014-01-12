/**
 * @file ButtonEventMonitor.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include "ButtonEventMonitor.h"

void ButtonEventMonitor::execute()
{
  update(getButtonState()[ButtonState::Chest], getButtonData().isPressed[ButtonData::Chest]);
  update(getButtonState()[ButtonState::LeftFoot], 
    getButtonData().isPressed[ButtonData::LeftFootLeft] || getButtonData().isPressed[ButtonData::LeftFootRight]);
  update(getButtonState()[ButtonState::RightFoot], 
    getButtonData().isPressed[ButtonData::RightFootLeft] || getButtonData().isPressed[ButtonData::RightFootRight]);
}

void ButtonEventMonitor::update(ButtonEvent& buttonEvent, bool pressed)
{
  buttonEvent = ButtonEvent::NONE;

  // update button state
  if(buttonEvent.isPressed != pressed) 
  {
    buttonEvent = pressed?ButtonEvent::PRESSED:ButtonEvent::RELEASED;
    buttonEvent.timeOfLastEvent = getFrameInfo().getTime();
    buttonEvent.isPressed = pressed;
  }

  // button is pressed not longer than 1s
  if(buttonEvent == ButtonEvent::RELEASED && getFrameInfo().getTime() < buttonEvent.timeOfLastEvent + 1000) {
    buttonEvent = ButtonEvent::CLICKED;
  }
}



