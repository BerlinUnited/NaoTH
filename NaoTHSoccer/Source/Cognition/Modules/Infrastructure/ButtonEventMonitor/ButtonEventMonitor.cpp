/**
 * @file ButtonEventMonitor.cpp
 *
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include "ButtonEventMonitor.h"

void ButtonEventMonitor::execute()
{
  // update Head Buttons
  update(getButtonState()[ButtonState::HeadFront], getButtonData().isPressed[ButtonData::HeadFront]);
  update(getButtonState()[ButtonState::HeadMiddle], getButtonData().isPressed[ButtonData::HeadMiddle]);
  update(getButtonState()[ButtonState::HeadRear], getButtonData().isPressed[ButtonData::HeadRear]);

  // update Chest
  update(getButtonState()[ButtonState::Chest], getButtonData().isPressed[ButtonData::Chest]);

  // update feet
  update(getButtonState()[ButtonState::LeftFootLeft], getButtonData().isPressed[ButtonData::LeftFootLeft]);
  update(getButtonState()[ButtonState::LeftFootRight], getButtonData().isPressed[ButtonData::LeftFootRight]);

  update(getButtonState()[ButtonState::RightFootLeft], getButtonData().isPressed[ButtonData::RightFootLeft]);
  update(getButtonState()[ButtonState::RightFootRight], getButtonData().isPressed[ButtonData::RightFootRight]);

  // update hands
  update(getButtonState()[ButtonState::LeftHandBack], getButtonData().isPressed[ButtonData::LeftHandBack]);
  update(getButtonState()[ButtonState::LeftHandLeft], getButtonData().isPressed[ButtonData::LeftHandLeft]);
  update(getButtonState()[ButtonState::LeftHandRight], getButtonData().isPressed[ButtonData::LeftHandRight]);

  update(getButtonState()[ButtonState::RightHandBack], getButtonData().isPressed[ButtonData::RightHandBack]);
  update(getButtonState()[ButtonState::RightHandLeft], getButtonData().isPressed[ButtonData::RightHandLeft]);
  update(getButtonState()[ButtonState::RightHandRight], getButtonData().isPressed[ButtonData::RightHandRight]);
}

void ButtonEventMonitor::update(ButtonEvent& buttonEvent, bool pressed)
{
  // NOTE: carefully choosen magic numbers in ms
  // if the time between clicks is smaller than this, then the click counter is increased (e.g. for double cklicks)
  static const unsigned int maxTimeBetweenClicksInSequence = 350;
  // the maximal time between PRESSED and RELEASED for the event to count as a CLICK
  static const unsigned int maxTimeLengthOfClick = 1000;

  buttonEvent = ButtonEvent::NONE;

  // detect change in the status of the button
  if(buttonEvent.isPressed != pressed)
  {
    buttonEvent = pressed ? ButtonEvent::PRESSED : ButtonEvent::RELEASED;
    buttonEvent.isPressed = pressed;
  }

  // button is pressed not longer than 1s
  if(buttonEvent == ButtonEvent::RELEASED && buttonEvent.timeSinceEvent() > 50 && buttonEvent.timeSinceEvent() < maxTimeLengthOfClick) {
    buttonEvent = ButtonEvent::CLICKED;

    if(buttonEvent.clicksInSequence > 0  && buttonEvent.timeSinceClick() < maxTimeBetweenClicksInSequence) {
      // additional click inside same sequence occured
      buttonEvent.clicksInSequence += 1;
    } else {
      // start a click sequence with an initial click
      buttonEvent.clicksInSequence = 1;
    }
  }

  if(buttonEvent == ButtonEvent::CLICKED) {
    buttonEvent.timeOfLastClick = getFrameInfo().getTime();
  }

  if(buttonEvent != ButtonEvent::NONE) {
    buttonEvent.timeOfLastEvent = getFrameInfo().getTime();
  }

  buttonEvent.updateTime = getFrameInfo().getTime();
}



