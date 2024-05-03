/* 
 * File:   ButtonData.h
 * Author: thomas
 *
 * Created on 6. march 2009, 10:40
 */

#ifndef BUTTONDATA_H
#define BUTTONDATA_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

namespace naoth
{
  class ButtonData : public Printable
  {
  public:
    ButtonData();

    enum ButtonID
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

    static std::string getButtonName(ButtonID id);

    /**
    * The button is pressed in this frame.
    * NOTE: this is the actual data provided by the NAO
    */
    bool isPressed[numOfButtons];

    /**
    * The number of frames the button was contiguous pressed (isPressed[numOfButtons] is true). 
    * I.e., it is 1 already in the first pressed frame and 0 if isPressed[numOfButtons] is false.
    * NOTE: this is a calculated value.
    * TODO: remove or move to ButtonState.
    */
    int numOfFramesPressed[numOfButtons];

    /** 
    * Overall number of contiguous pressing events.
    * NOTE: this is a calculated value.
    * TODO: remove or move to ButtonState.
    */
    int eventCounter[numOfButtons];


    void updatePressed(ButtonID id, bool pressed) {

      // update calculated values
      if(pressed) {
        numOfFramesPressed[id]++;

        // count new event if the button was not pressed in the last frame
        if(!isPressed[id]) {
          eventCounter[id]++;
        }
      } else { 
        // the button was released => reset the counter
        numOfFramesPressed[id] = 0;
      }

      isPressed[id] = pressed;
    }

    virtual void print(std::ostream& stream) const;
  };

  template<>
  class Serializer<ButtonData>
  {
    public:
    static void serialize(const ButtonData& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, ButtonData& representation);
  };

} // namespace naoth

#endif  /* BUTTONDATA_H */

