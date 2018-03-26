/**
 * @file BDRControlCommand.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 */

#ifndef _BDRControlCommand_H_
#define _BDRControlCommand_H_

#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include <Representations/Infrastructure/FrameInfo.h>

class BDRControlCommand : public naoth::Printable
{
public:

  BDRControlCommand() 
    : behaviorMode(DO_NOTHING)
  {}

  enum BehaviorMode
  {
    DO_NOTHING,
    AUTONOMOUS_PLAY,
    WARTUNG,
    NUMBER_OF_BEHAVIOR_MODE_TYPE,
  } behaviorMode;

  naoth::FrameInfo frameInfoWhenUpdated;

    virtual void print(std::ostream& stream) const
  {
    stream << "BDRControlCommand" << std::endl;
    stream << "BehaviorMode = " << getBehaviorModeName(behaviorMode) << std::endl;
  }


  static std::string getBehaviorModeName(BehaviorMode id);
};


namespace naoth
{
  template<>
  class Serializer<BDRControlCommand>
  {
  public:
    static void serialize(const BDRControlCommand& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, BDRControlCommand& representation);
  };
}

#endif // _BDRControlCommand_H_
