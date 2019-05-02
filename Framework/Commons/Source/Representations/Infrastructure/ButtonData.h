/* 
 * File:   ButtonData.h
 * Author: thomas
 *
 * Created on 6. march 2009, 10:40
 */

#ifndef _BUTTONDATA_H
#define _BUTTONDATA_H

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

    /** Whether the button is pressed in this frame. */
    bool isPressed[numOfButtons];
    /** The number of frames the button was contiguous pressed (isPressed[numOfButtons] is true). 
    * I.e., it is 1 already in the first pressed frame and 0 if isPressed[numOfButtons] is false.*/
    int numOfFramesPressed[numOfButtons];
    /** Overall number of contiguous pressing events */
    int eventCounter[numOfButtons];

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

#endif  /* _BUTTONDATA_H */

