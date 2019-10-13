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
  update(getButtonState()[ButtonState::HeadMiddle],
         getButtonData().isPressed[ButtonData::HeadMiddle] || getButtonData().isPressed[ButtonData::HeadFront]);
  update(getButtonState()[ButtonState::HeadMiddle],
         getButtonData().isPressed[ButtonData::HeadMiddle] || getButtonData().isPressed[ButtonData::HeadMiddle]);
  update(getButtonState()[ButtonState::HeadMiddle],
         getButtonData().isPressed[ButtonData::HeadMiddle] || getButtonData().isPressed[ButtonData::HeadRear]);

  // update Chest
  update(getButtonState()[ButtonState::Chest], getButtonData().isPressed[ButtonData::Chest]);

  // update feet
  // TODO 2019 Benji: provide ButtonData for bumper separately for proprioception
  update(getButtonState()[ButtonState::LeftFoot],
         getButtonData().isPressed[ButtonData::LeftFootLeft] || getButtonData().isPressed[ButtonData::LeftFootRight]);
  update(getButtonState()[ButtonState::RightFoot],
         getButtonData().isPressed[ButtonData::RightFootLeft] || getButtonData().isPressed[ButtonData::RightFootRight]);

  // TODO 2019 Benji Hands are missing
}

void ButtonEventMonitor::update(ButtonEvent& buttonEvent, bool pressed)
{
  const unsigned int maxSequenceLength = 350;

  buttonEvent = ButtonEvent::NONE;

  // detect change in the status of the button
  if(buttonEvent.isPressed != pressed)
  {
    buttonEvent = pressed ? ButtonEvent::PRESSED : ButtonEvent::RELEASED;
    buttonEvent.isPressed = pressed;
  }

  // button is pressed not longer than 1s
  if(buttonEvent == ButtonEvent::RELEASED && getFrameInfo().getTime() < buttonEvent.timeOfLastEvent + 1000) {
    buttonEvent = ButtonEvent::CLICKED;

    if(buttonEvent.clicksInSequence > 0  && getFrameInfo().getTime() < buttonEvent.timeOfLastClick + maxSequenceLength) {
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

  if(!(buttonEvent == ButtonEvent::NONE)) {
    buttonEvent.timeOfLastEvent = getFrameInfo().getTime();
  }
}



