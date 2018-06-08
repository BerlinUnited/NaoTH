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
  update(getButtonState()[ButtonState::HeadMiddle],
    getButtonData().isPressed[ButtonData::HeadMiddle] || getButtonData().isPressed[ButtonData::HeadMiddle]);
}

void ButtonEventMonitor::update(ButtonEvent& buttonEvent, bool pressed)
{
  const unsigned int doubleClickWaitTime = 300;


  buttonEvent = ButtonEvent::NONE;

  // detect change in the status of the button
  if(buttonEvent.isPressed != pressed)
  {
    buttonEvent = pressed?ButtonEvent::PRESSED:ButtonEvent::RELEASED;
    buttonEvent.isPressed = pressed;
  }

  // button is pressed not longer than 1s
  if(buttonEvent == ButtonEvent::RELEASED && getFrameInfo().getTime() < buttonEvent.timeOfLastEvent + 1000) {
    if(buttonEvent.clickNeedsProcessing && getFrameInfo().getTime() < buttonEvent.timeOfLastClick + doubleClickWaitTime) {
      // double click occured
      buttonEvent.clickNeedsProcessing = false;
      buttonEvent = ButtonEvent::DOUBLE_CLICKED;
    } else {
      // remember that there was a click in this frame and make sure this event is process later
      buttonEvent.clickNeedsProcessing = true;
      buttonEvent.timeOfLastClick = getFrameInfo().getTime();
    }
  }


  if(buttonEvent == ButtonEvent::NONE) {
    // check if we have to process an old click event after waiting long enough for a potential double click
    unsigned int clickSequenceTime = getFrameInfo().getTime() - buttonEvent.timeOfLastClick;
    if(buttonEvent.clickNeedsProcessing && clickSequenceTime > doubleClickWaitTime) {
      buttonEvent.clickNeedsProcessing = false;
      buttonEvent = ButtonEvent::CLICKED;
    }
  }

  if(!(buttonEvent == ButtonEvent::NONE)) {
    buttonEvent.timeOfLastEvent = getFrameInfo().getTime();
  }
}



