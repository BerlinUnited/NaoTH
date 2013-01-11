/* 
 * File:   ButtonData.h
 * Author: thomas
 *
 * Created on 6. march 2009, 10:40
 */

#ifndef _BUTTONDATA_H
#define _BUTTONDATA_H

#include "Tools/DataStructures/Printable.h"

namespace naoth
{
  class ButtonData : public Printable
  {
  public:
    ButtonData();
    virtual ~ButtonData();

    enum ButtonID
    {
      Chest,
      LeftFootLeft,
      LeftFootRight,
      RightFootLeft,
      RightFootRight,
      numOfButtons
    };

    static std::string getButtonName(ButtonID id);

    /** Whether the button is pressed in this frame. */
    bool isPressed[numOfButtons];
    /** Overall number of contiguous pressing events */
    int eventCounter[numOfButtons];
    /** The number of frames the button was contiguous pressed. Should be 1 already
     * in the first pressed frame. */
    int numOfFramesPressed[numOfButtons];

    virtual void print(std::ostream& stream) const;
  private:

  };
}

#endif  /* _BUTTONDATA_H */

