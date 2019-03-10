//--------------------------------------------------------------------------------------------------
//
// @file JoypadData.h
// @author <a href="mailto:albert@informatik.hu-berlin.de">Andreas Albert</a>
//
// Joypad/Gamepad data declarations
//
//--------------------------------------------------------------------------------------------------
//
#ifndef _JOYPADDATA_H
#define _JOYPADDATA_H
//==================================================================================================
//--------------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector2.h"
//--------------------------------------------------------------------------------------------------
namespace naoth
{
  namespace joypads
  {
    struct UnifiedJoypadControls
    {
      bool isValid{false};
      struct Buttons
      {
        bool select{false};
        bool start{false};
        bool A{false};
        bool B{false};
        bool X{false};
        bool Y{false};
        bool LB{false}; // left bumper
        bool RB{false}; // right bumper
        bool LT{false}; // left trigger  (as binary - usually analog values)
        bool RT{false}; // right trigger (as binary - usually analog values)
                        // have more? - put them here
      } button;
      struct Triggers
      {
        double LT{0.0};
        double RT{0.0};
      } trigger;
      struct Sticks
      {
        Vector2d L{0, 0};   // left stick  (analog) - normalized -1 .. 0 .. 1
        Vector2d R{0, 0};   // right stick (analog) - normalized -1 .. 0 .. 1
      } stick;
      int dpad{0x0F}; // directional pad - usually 0..7 (N, NE, E...), 15=default/no direction
    };
  } // namespace joypads
//--------------------------------------------------------------------------------------------------
  class JoypadData : public Printable
  {
  public:
    joypads::UnifiedJoypadControls controls;
    virtual void print(std::ostream& stream) const;
    JoypadData();
    virtual ~JoypadData();
  };
} // namespace naoth
//--------------------------------------------------------------------------------------------------
//==================================================================================================
#endif  // _JOYPADDATA_H
