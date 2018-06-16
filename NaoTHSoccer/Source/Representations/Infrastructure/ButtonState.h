/**
 * @file ButtonState.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _ButtonState_H
#define _ButtonState_H

#include <Tools/DataStructures/Printable.h>

class ButtonEvent
{
public:
  enum EventType
  {
    NONE,
    PRESSED,
    RELEASED,
    CLICKED  // button is pressed not longer than 1s
  } eventState;

  unsigned int timeOfLastEvent;
  unsigned int timeOfLastClick;
  unsigned int clicksInSequence;
  bool isPressed;

  ButtonEvent() : eventState(NONE), timeOfLastEvent(0), timeOfLastClick(0), clicksInSequence(0), isPressed(false) {}
  void operator=(EventType id) { eventState = id; }
  bool operator==(EventType id) const { return this->eventState == id; }

  bool isSingleClick() const {
    return eventState == CLICKED && clicksInSequence == 1;
  }

  bool isDoubleClick() const {
    return eventState == CLICKED && clicksInSequence == 2;
  }

  std::string print() const
  {
    switch(eventState) {
      case PRESSED: return "PRESSED";
      case RELEASED: return "RELEASED";
      case CLICKED: return "CLICKED";
      default: return "NONE";
    }
  }
};

class ButtonState: public naoth::Printable
{
public:
  enum ButtonType
  {
    Chest,
    LeftFoot,
    RightFoot,
    HeadMiddle,
    numOfButtons
  };

  ButtonEvent buttons[numOfButtons];

  inline ButtonEvent& operator[](ButtonType id){ return buttons[id]; }
  inline const ButtonEvent& operator[](ButtonType id) const{ return buttons[id]; }

  virtual void print(std::ostream& stream) const
  {
    stream << "Chest: " << buttons[Chest].print() << ", timestamp: " << buttons[Chest].timeOfLastEvent << std::endl;
  }
};

#endif // _ButtonState_H
