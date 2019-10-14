/**
 * @file ButtonState.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _ButtonState_H
#define _ButtonState_H

#include <Tools/DataStructures/Printable.h>

class ButtonEvent {
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
    void operator=(EventType id) {
      eventState = id;
    }
    bool operator==(EventType id) const {
      return this->eventState == id;
    }

    bool isSingleClick() const {
      return eventState == CLICKED && clicksInSequence == 1;
    }

    bool isDoubleClick() const {
      return eventState == CLICKED && clicksInSequence == 2;
    }

    std::string print() const {
      switch(eventState) {
        case PRESSED:
          return "PRESSED";
        case RELEASED:
          return "RELEASED";
        case CLICKED:
          return "CLICKED";
        default:
          return "NONE";
      }
    }
};

class ButtonState: public naoth::Printable {
  public:
    enum ButtonType
    {
      Chest,

      LeftFootLeft,
      LeftFootRight,
      RightFootLeft,
      RightFootRight,

      HeadFront,
      HeadMiddle,
      HeadRear,

      LeftHandBack,
      LeftHandLeft,
      LeftHandRight,
      
      RightHandBack,
      RightHandLeft,
      RightHandRight,
      numOfButtons
    };

    ButtonEvent buttons[numOfButtons];

    inline ButtonEvent& operator[](ButtonType id) {
      return buttons[id];
    }
    inline const ButtonEvent& operator[](ButtonType id) const {
      return buttons[id];
    }

    virtual void print(std::ostream& stream) const {
      stream << "Chest: "      << buttons[Chest].print()      << ", timestamp: " << buttons[Chest].timeOfLastEvent      << std::endl;
      stream << "LeftFootLeft: " << buttons[LeftFootLeft].print() << ", timestamp: " << buttons[LeftFootLeft].timeOfLastEvent << std::endl;
      stream << "LeftFootRight: " << buttons[LeftFootRight].print() << ", timestamp: " << buttons[LeftFootRight].timeOfLastEvent << std::endl;
      stream << "RightFootLeft: " << buttons[RightFootLeft].print() << ", timestamp: " << buttons[RightFootLeft].timeOfLastEvent << std::endl;
      stream << "RightFootRight: " << buttons[RightFootRight].print() << ", timestamp: " << buttons[RightFootRight].timeOfLastEvent << std::endl;
      stream << "HeadFront: "  << buttons[HeadFront].print()  << ", timestamp: " << buttons[HeadFront].timeOfLastEvent  << std::endl;
      stream << "HeadMiddle: " << buttons[HeadMiddle].print() << ", timestamp: " << buttons[HeadMiddle].timeOfLastEvent << std::endl;
      stream << "HeadRear: "   << buttons[HeadRear].print()   << ", timestamp: " << buttons[HeadRear].timeOfLastEvent   << std::endl;
      //TODO add hands
    }
};

#endif // _ButtonState_H
