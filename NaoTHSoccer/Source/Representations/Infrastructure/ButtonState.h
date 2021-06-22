/**
 * @file ButtonState.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _ButtonState_H
#define _ButtonState_H

#include <Tools/Debug/NaoTHAssert.h>
#include <Tools/DataStructures/Printable.h>

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
      return this->eventState == id;
    }

    bool operator!=(EventType id) const {
      return this->eventState != id;
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
