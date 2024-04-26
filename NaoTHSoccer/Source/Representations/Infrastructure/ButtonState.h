/**
 * @file ButtonState.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef BUTTON_STATE_H
#define BUTTON_STATE_H

#include <Tools/Debug/NaoTHAssert.h>
#include <Tools/DataStructures/Printable.h>

#include <iomanip> // for std::setw

class ButtonEvent 
{
  public:
    enum EventType
    {
      NONE,
      PRESSED,
      RELEASED,
      CLICKED,  // button is pressed not longer than 1s
      numOfButtonEvent
    } eventState;

    unsigned int updateTime; // timestamp when this representation was updated
    
    unsigned int timeOfLastEvent; // then did the last event occur
    unsigned int timeOfLastClick; // this is used to count clicks in a sequence
    unsigned int clicksInSequence;

    bool isPressed; // this is the current state of the button, which is used to detect events (changes in the state)

    ButtonEvent() : eventState(NONE), timeOfLastEvent(0), timeOfLastClick(0), clicksInSequence(0), isPressed(false) {}
    
    void operator=(EventType id) {
      eventState = id;
    }
    
    bool operator==(EventType id) const {
      return eventState == id;
    }

    bool operator!=(EventType id) const {
      return eventState != id;
    }

    bool isSingleClick() const {
      return eventState == CLICKED && clicksInSequence == 1;
    }

    bool isDoubleClick() const {
      return eventState == CLICKED && clicksInSequence == 2;
    }

    unsigned int timeSinceEvent() const {
      ASSERT(updateTime >= timeOfLastEvent);
      return updateTime - timeOfLastEvent;
    }

    unsigned int timeSinceClick() const {
      ASSERT(updateTime >= timeOfLastClick);
      return updateTime - timeOfLastClick;
    }

    std::string print() const {
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

    inline static std::string toString(ButtonType buttonType) {
      switch(buttonType) {
        case Chest: return "Chest";

        case LeftFootLeft: return "LeftFootLeft";
        case LeftFootRight: return "LeftFootRight";
        case RightFootLeft: return "RightFootLeft";
        case RightFootRight: return "RightFootRight";

        case HeadFront: return "HeadFront";
        case HeadMiddle: return "HeadMiddle";
        case HeadRear: return "HeadRear";

        case LeftHandBack: return "LeftHandBack";
        case LeftHandLeft: return "LeftHandLeft";
        case LeftHandRight: return "LeftHandRight";

        case RightHandBack: return "RightHandBack";
        case RightHandLeft: return "RightHandLeft";
        case RightHandRight: return "RightHandRight";
        default: 
          ASSERT_MSG(false, "Unknown ButtonType");
      }

      return "Unknown ButtonType";
    }

    ButtonEvent buttons[numOfButtons];

    inline ButtonEvent& operator[](ButtonType id) {
      return buttons[id];
    }
    inline const ButtonEvent& operator[](ButtonType id) const {
      return buttons[id];
    }

    virtual void print(std::ostream& stream) const 
    {
      stream << std::left << std::setw(16) << "Button"     << " | "
             << std::left << std::setw(7)  << "Pressed"    << " | "
             << std::left << std::setw(10) << "Event"      << " | "
             << std::left << std::setw(10) << "Timestamp "
             << std::endl;

      stream << "----------------------------------------------------------" << std::endl;

      // list all buttons
      for(int i = 0; i < numOfButtons; ++i) {
        const ButtonType type = static_cast<ButtonType>(i);
        const ButtonEvent& button = buttons[type];

        stream << std::left << std::setw(16) << toString(type)                    << " | "
               << std::left << std::setw(7)  << (button.isPressed?"   X   ":" ")  << " | "
               << std::left << std::setw(10) <<  button.print()                   << " | "
               << std::left << std::setw(10) <<  button.timeOfLastEvent
               << std::endl;
      }
    }
};

#endif // BUTTONSTATE_H
